#include "ServerManager.hpp"

bool isExit = false;
// 🚀 FIXME:: Ask teammate about storing the info in config
//          ip address and port is needed to initialize the server
//          Discuss how address and ip address (obtained from server.config)are stored
//          needs to change the server constructor accordingly

ServerManager::ServerManager(const std::vector<ServerTraits>& cnf)
{
    try {
        for (size_t i = 0; i < cnf.size(); ++i) 
        {
            servers.push_back(Server(cnf[i])); // Create servers

            // Add server sockets to poll list
            struct pollfd pfd;
            pfd.fd = servers.back().getServerFd();
            pfd.events = POLLIN;
            pfd.revents = 0;

            sockets.push_back(pfd);  // Now push the initialized struct into the vector

            std::cout << "✅ Server is listening on port " << cnf[i].listen_port << std::endl;
        }
    } catch (const std::exception &e) {
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
void ServerManager::run()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handle_exit);

    while(!isExit)
    {
        int eventCount = poll(sockets.data(), sockets.size(), -1);
        if (eventCount < 0) 
            throw std::runtime_error("Poll Error");
        for (size_t i = 0; i < sockets.size(); ++i) 
        {
            struct pollfd &pfd = sockets[i];
            // Accept new client connections
            if (pfd.revents & POLLIN && i < servers.size()) 
            {
                int clientFd = accept(pfd.fd, servers[i].getAddress(),servers[i].getAddrlen());
                if (clientFd < 0) 
                    throw std::runtime_error("Accept Error");
                // Add new client to poll list
                struct pollfd new_pfd;
                new_pfd.fd = clientFd;
                new_pfd.events = POLLIN | POLLOUT;
                new_pfd.revents = 0;
                sockets.push_back(new_pfd);
                clientBuffers[clientFd] = "";
                isReqComplete[clientFd] = false;
            }
            //Read data from clients
            else if (pfd.revents & POLLIN && i >= servers.size())
            {
                char buffer[BUFFER_SIZE] = {0};
                int bytesRead = recv(pfd.fd, buffer, BUFFER_SIZE - 1, 0);
                
                if (bytesRead <= 0) {
                    // Client disconnected
                    std::cout << "❌ Client disconnected: FD " << pfd.fd << std::endl;
                    close(pfd.fd);
                    sockets.erase(sockets.begin() + i);
                    clientBuffers.erase(pfd.fd);
                    --i;
                    continue;
                }
                
                clientBuffers[pfd.fd] += std::string(buffer, bytesRead);
                    //check whether the HTTP request is complete or partial by analysing the client data
                isReqComplete[pfd.fd] = partialRequest(clientBuffers[pfd.fd]);
                std::cout << "📩 Received from client FD " << pfd.fd << ": " << buffer << std::endl;
            }
                // Send response to clients
                // 🚀 FIXME: 
                //      1.Ask teammate about handling HTTP request parsing,
                //      2.Need confirmation on expected format of response
                //      3.Implementation of error Handling and custom error Pages


            else if (pfd.revents & POLLOUT && i >= servers.size()&& isReqComplete[pfd.fd] == true) 
            {
                std::cout << "Current request buffer for FD " << pfd.fd << ": " << clientBuffers[pfd.fd] << std::endl;
                Response response = ManageRequest(clientBuffers[pfd.fd]);

                // if (response.empty()) {
                //     response = generateErrorResponse("400", "Bad Request");
                // }

                // std::cout << "📤 Sent response to FD " << pfd.fd << ": " << response << std::endl;
                // send(pfd.fd, response.c_str(), response.length(), 0);

                close(pfd.fd);
                sockets.erase(sockets.begin() + i);
                clientBuffers.erase(pfd.fd);
                --i;
            }

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
		if ((((*it).getConf().listen_address == address)
			|| ((*it).getConf().listen_address == htonl(INADDR_ANY)))
			&& (*it).getConf().listen_port == port)
			    return (it);
            bool foundAddStr = std::find((*it).getConf().server_name.begin(),
            (*it).getConf().server_name.end(), addStr) != (*it).getConf().server_name.end();
            bool foundUnderscore = std::find((*it).getConf().server_name.begin(),
                            (*it).getConf().server_name.end(), "_") != (*it).getConf().server_name.end();
            bool nameMatch = foundAddStr || foundUnderscore;
            bool portMatch = (*it).getConf().listen_port == port;

        if (nameMatch && portMatch)
                return (it);
	}
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
    res.setResponseHeader(code, mssg);
    if(conf.error_pages.find(code) != conf.error_pages.end())
    {
        res.setErrBody(conf.error_pages.find(code)->second, req);
    }
    else
    {
        res.setErrBody(getErrPage(code,mssg), req);
    }    
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
    
	const ServerTraits& conf = (*serv_it).getConf();
	normalizeUrl(url);
	ServerRoute route = getRoute(url, conf);
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