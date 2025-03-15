#include "ServerManager.hpp"

bool isExit = false;
// 🚀 FIXME:: Ask teammate about storing the info in config
//          ip address and port is needed to initialize the server
//          Discuss how address and ip address (obtained from server.config)are stored
//          needs to change the server constructor accordingly

/**
 * Constructs a ServerManager with the given server configurations.
 *
 * @param cnf A vector of ServerTraits objects, representing the server configurations.
 */
ServerManager::ServerManager(const std::vector<ServerTraits>& cnf)
{
    try {
        // Create servers and initialize their sockets
        for (size_t i = 0; i < cnf.size(); ++i) {
            // Create a new server with the given configuration
            servers.push_back(Server(cnf[i]));
            std::vector<Server>::iterator it;
        for (it = servers.begin(); it != servers.end();it++)
        {
            std::cout << (*it).getConf().listen_port;
        }

            // Initialize the server socket for polling
            struct pollfd socketConfig;
            socketConfig.fd = servers.back().getServerFd();
            socketConfig.events = POLLIN;
            socketConfig.revents = 0;

            // Add the socket to the poll list
            sockets.push_back(socketConfig);

            // Log a success message
            std::cout << "✅ Server is listening on port " << cnf[i].listen_port << std::endl;
        }
    } catch (const std::exception& e) {
        // Log an error message and re-throw the exception
        std::cerr << "❌ Error: " << e.what() << std::endl;
        throw;
    }
}
ServerManager::ServerManager(const ServerManager& src)
{
	if (this == &src)
		return ;
	*this = src;
}

ServerManager& ServerManager::operator = (const ServerManager& src)
{
	if (this == &src)
		return *this;
	this->sockets = src.sockets;
	return *this;
}
ServerManager::~ServerManager()
{
    for (size_t i = 0; i < sockets.size(); ++i) {
        close(sockets[i].fd);
    }
}
/**
 * ServerManager Run Method
 *
 * This method is the main loop of the ServerManager class, responsible for handling
 * incoming client connections, reading data from clients, and sending responses back
 * to clients.
 */
void ServerManager::run()
{
    // Signal Handling
    signal(SIGPIPE, SIG_IGN);  // Ignore SIGPIPE signals
    signal(SIGINT, handle_exit);  // Handle SIGINT signals using the handle_exit function

    // Main Loop
    while (!isExit)
    {
        try
        {
            // Polling
            int eventCount = poll(sockets.data(), sockets.size(), -1);
            if (eventCount < 0)
                throw std::runtime_error("Poll Error");
            // Event Handling
            for (size_t i = 0; i < sockets.size(); ++i)
            {
                struct pollfd& pfd = sockets[i];
                // New Client Connections
                if (pfd.revents & POLLIN && i < servers.size())
                {
                    std::cout << "inside accepting client loop" << std::endl;
                    // Accepting New Connections
                    Client client = servers[i].acceptNewClient();
                    // Add New Client to Poll List
                    struct pollfd new_pfd;
                    client.addPoll(new_pfd);
                    sockets.push_back(new_pfd);
                    std::cout << "Added client FD " << new_pfd.fd << " to poll list" << std::endl;
                    clients.push_back(client);
                    printClients(clients);

                    // Initialize Client Buffers
                    clientBuffers[client.getFd()] = "";
                    isReqComplete[client.getFd()] = false;
                }
                // Reading Data from Clients
                else if (pfd.revents & POLLIN && i >= servers.size())
                {
                    try
                    {    
                        getClientByFd(clients, pfd.fd)->readData(clientBuffers[pfd.fd]);
                        std::cout << clientBuffers[pfd.fd] << std::endl;
                        // Check for Request Completion
                        isReqComplete[pfd.fd] = partialRequest(clientBuffers[pfd.fd]);
                        std::cout << isReqComplete[pfd.fd] << std::endl;
                    }
                    catch (const std::exception& e)
                    {
                        // Handle Read Error
                        std::cerr << "Error reading from client:/client disconnected " << e.what() << std::endl;
                        close(pfd.fd);
                        clientBuffers[pfd.fd].clear();
                        sockets.erase(sockets.begin() + i);
                        // clients.erase(clients.begin() + i);
                        clientBuffers.erase(pfd.fd);
                        --i;
                    }
                }
                // Sending Responses to Clients
                else if (pfd.revents & POLLOUT && i >= servers.size() && isReqComplete[pfd.fd] == true)
                {
                    try
                    {
                        // Send Response
                        Response response = ManageRequest(clientBuffers[pfd.fd]);
                        send(pfd.fd, response.getRes().c_str(), response.getRes().length(), 0);

                        // Close Socket
                        close(pfd.fd);
                        sockets.erase(sockets.begin() + i);
                        // removeClientByFd(clients, pfd.fd);
                        clientBuffers.erase(pfd.fd);
                        --i;
                    }
                    catch (const std::exception& e)
                    {
                        // Handle Send Error
                        std::cerr << "Error sending response to client: " << e.what() << std::endl;
                        close(pfd.fd);
                        sockets.erase(sockets.begin() + i);
                        removeClientByFd(clients, pfd.fd);
                        clientBuffers.erase(pfd.fd);
                        --i;
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            // Handle Main Loop Error
            std::cerr << "Error in main loop: " << e.what() << std::endl;
        }
    }
}
/*
* Find the server that matches the host and port
* If the host is an IP address, match the address and port
* If the host is a domain name, match the server name and port
*/
static std::vector<Server>::iterator findServer(std::vector<Server>::iterator start, std::vector<Server>::iterator end, const std::string& host)
{
    std::vector<Server>::iterator it;  
  	ft::string hst = host;
	in_port_t port = 0;
	in_addr_t address = 0;
    if (hst.find(':') == std::string::npos)
        throw std::runtime_error("400");
    string addStr = hst.substr(0, hst.find(':'));
    string portStr = hst.substr(hst.find(':') + 1);

    if (std::count(host.begin(), host.end(), '.') == 3)
	{
		setAddress(hst, address, port);	
	} else {
        setAddress(portStr, address, port);
    }

    for (it = start; it != end; ++it)
	{
        	std::cout << "Checking HOST: Address=" << address
          << ", Port=" << port << std::endl;
        std::cout << "Checking Server: Address=" << (*it).getConf().listen_address
          << ", Port=" << (*it).getConf().listen_port << std::endl;
        if ((((*it).getConf().listen_address == address) || ((*it).getConf().listen_address == htonl(INADDR_ANY))) && (*it).getConf().listen_port == port)
            return (it);
        bool foundAddStr = std::find((*it).getConf().server_name.begin(),
                                     (*it).getConf().server_name.end(), addStr) != (*it).getConf().server_name.end();
        bool foundUnderscore = std::find((*it).getConf().server_name.begin(),
                                         (*it).getConf().server_name.end(), "_") != (*it).getConf().server_name.end();
        bool nameMatch = foundAddStr || foundUnderscore;
        bool portMatch = (*it).getConf().listen_port == port;

        if (nameMatch && portMatch)
        {
            std::cout << "MATCHING" << std::endl;
            return (it);
        }
	}
    std::cout << "NO MATCHING" << std::endl;
	return (it);
}

static string getDir(string path)
{
	size_t pos = path.rfind('/');
	if (pos == std::string::npos)
		throw std::runtime_error("400");
	else
		path = path.substr(0, pos);
	return (path);
}
ServerRoute ServerManager::getRoute(string& url, const ServerTraits& conf)
{
    // Iterator to find the route
    std::map<ft::string, ServerRoute>::const_iterator route_it;
    
    // Try to find the route for the given URL
    route_it = conf.routes.find(url);
    
    // If no route is found, keep stripping the URL until a match is found or the URL is empty
    while (route_it == conf.routes.end() && !url.empty())
    {
        url = getDir(url);  // Remove the last directory or file from the URL
        route_it = conf.routes.find(url);  // Try to find the route again
    }
    
    // If the URL is empty and no route is found, try to find the root route
    if (route_it == conf.routes.end() && url.empty())
    {
        url = "/";
        route_it = conf.routes.find(url);
        
        // If no root route is found, throw an error
        if (route_it == conf.routes.end())
            throw ErrorPage(conf, "500");
    }
    
    // Return the found route
    return route_it->second;
}
const	std::string	getErrPage(const std::string& code, const std::string& mssg)
{
	// if (conf.error_pages.find(code) != conf.error_pages.end())
	// 	return (conf.error_pages.find(code)->second);

	std::string	html = "<!DOCTYPE html>"
						"<html lang=\"en\">"
						"<head>"
						"	<meta charset=\"UTF-8\">"
						"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
						"	<title>" + code + " " + mssg + "</title>"
						""
						"	<style>"
						"		h1 {"
						"			text-align: center;"
						"			margin-top: 40px;"
						"			color: red;"
						"		}"
						"		h3 {"
						"			text-align: center;"
						"		}"
						""
						"	</style>"
						"</head>"
						"<body>"
						"	<h1>" + code + " " + mssg + "</h1>"
						"	<hr>"
						"	<h3>webserv server</h3>"
						"</body>"
						"</html>";

	return (html);
}
static void setErrPage(Response &res, const Request &req, const std::string& code, const std::string &mssg, const ServerTraits& conf)
{
    (void)conf;
    res.setResponseHeader(code, mssg);
    // if(conf.error_pages.find(code) != conf.error_pages.end())
    // {
    //     res.setErrBody(conf.error_pages.find(code)->second, req);
    // }
    // else
        res.setErrBody(getErrPage(code,mssg), req);
       
}

void ServerManager::ProcessResponse(Request &request)
{
    std::string host = request.getHost();
    const ft::string& urlx = request.getReqUrl();
	string url = urlx;
    if (host.empty())
        throw std::runtime_error("400");
            std::vector<Server>::iterator serv_it = findServer(
                servers.begin(), servers.end(), host);
        if (serv_it == servers.end())
        {
            std::cout << "Server not found" << std::endl;
            throw std::runtime_error("404");
    }
    //Get the Server Configuration
	const ServerTraits& conf = (*serv_it).getConf();
    
	normalizeUrl(url);
    string routeUrl = url;
    ServerRoute route = getRoute(url, conf);
    std::cout << route.root << std::endl;
    // checkClientBodySize(request, conf);
    // string path = constructFullPath(route, url);

    // throwIfnotAllowed(url, conf, request);

	// if (redirect(route, res))
	// 	return ;

	// handleRequestType(request, res, path, route, conf);
}
void ServerManager::normalizeUrl(string& url)
{
	if (url.back() == '/' && url.size() > 1)
		url.resize(url.size() - 1);
}
void setDefaultErrPage(Response &res, const Request &req, const std::string& code, const std::string &mssg)
{
    res.setResponseHeader(code, mssg);
    res.setErrBody(getErrPage(code,mssg), req);
}
Response ServerManager::ManageRequest(const std::string& buffer)
{
    Response response;
    Request request(buffer);
    std::string arr[] = {"400", "403", "404", "405", "500", "504"};
    std::string msgArr[] = {"Bad Request", "Forbidden", "Not Found", "Method Not Allowed",
        "Internal Server Error", "Gateway Timeout"};

    try
    {
        ProcessResponse(request);
    }
    catch(const ErrorPage& e)
    {
        setErrPage(response, request, e.what(), msgArr[(std::find(arr, arr + 6, e.what()) - arr)], e.getConf());
    }  
    catch (const std::runtime_error& e)
    {
       setDefaultErrPage(response, request, e.what(),msgArr[(std::find(arr, arr + 6, e.what()) - arr)]);
    }
    catch (const std::exception& e)
    {
        setDefaultErrPage(response, request, "500", "Internal Server Error");
    }
    return response;
}

// Graceful shutdown handler
void handle_exit(int sig)
{
    (void)sig;
    std::cout << "Shutting down the server..." << std::endl;
    isExit = true; // Set a flag to stop the server loop
    exit(0);
}
bool	ServerManager::partialRequest(std::string &buff)
{
    std::cout << buff << std::endl;
    // Check if the buffer contains the complete headers
    if (buff.find("\r\n\r\n") == std::string::npos)
		return (false);
    // Return false if the buffer is empty (no data)
	if (buff.empty())
		return (false);
    // 🚀 FIXME 1.// Handle chunked transfer encoding
    //          2.// Check if the request is a POST and the content length is not fully received
    return (true);
}