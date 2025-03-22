#include "ServerManager.hpp"

bool isExit = false;

/**
 * Constructs a ServerManager with the given server configurations.
 *
 * @param cnf A vector of ServerTraits objects, representing the server configurations.
 */
ServerManager::ServerManager(const std::vector<ServerTraits>& cnf)
{
    try {
        // Create servers and initialize their sockets
        for (size_t i = 0; i < cnf.size(); ++i) 
        {
            // Create a new server with the given configuration
            servers.push_back(Server(cnf[i]));            std::vector<Server>::iterator it;

            // Initialize the server socket for polling
            struct pollfd socketConfig;
            socketConfig.fd = servers.back().getServerFd();
            socketConfig.events = POLLIN;
            socketConfig.revents = 0;

            // Add the socket to the poll list
            sockets.push_back(socketConfig);

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
std::string	ServerManager::strToUpper(std::string str)
{
    for(size_t i = 0; i < str.length(); i++) {
        str[i] = toupper(str[i]);
    }
	return (str);
}
void	ServerManager::parseEnv(char **rawEnv)
{
	std::string str1;
	std::string envStr;
	bool	flag = false;

	for (size_t i = 0; rawEnv[i]; i++)
	{
		envStr += rawEnv[i];
		envStr += "\n";
	}
	std::stringstream str(envStr);
	while (getline(str, str1, '\n'))
	{
		std::stringstream line(str1);
		std::string key, value;
		getline(line, key, '=');
		getline(line, value);
		std::map<std::string, std::string>::iterator it;
		for (it = envMap.begin(); it != envMap.end(); ++it)
		{
			key = strToUpper(key);
			if (key == it->first)
				flag = true;
		}
		if (flag)
			continue;
		envMap[key] = value;
	}
}
void ServerManager::run(char **envp)
{
    parseEnv(envp);
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
                    
                    // Accepting New Connections
                    Client client = servers[i].acceptNewClient();
                    // Add New Client to Poll List
                    struct pollfd new_pfd;
                    client.addPoll(new_pfd);
                    sockets.push_back(new_pfd);
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
                        // Check for Request Completion
                        isReqComplete[pfd.fd] = partialRequest(clientBuffers[pfd.fd]);
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
        if ((((*it).getConf().listen_address == address) || ((*it).getConf().listen_address == htonl(INADDR_ANY))) && (*it).getConf().listen_port == port)
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
        {
            throw ErrorPage(conf, "404 Not Found");
        }
    }
    // Return the found route
    return route_it->second;
}

/*
* Check if the request is allowed for the given URL
* If the request type is not allowed, throw a 405 error
* curl -X POST http://localhost:8080 -d "name=John&age=30"
*/
void ServerManager::throwIfnotAllowed(const string& url, const ServerTraits& conf,
	const Request& request)
{
	std::map<ft::string, ServerRoute>::const_iterator route_it;
	route_it = conf.routes.find(url);
	if (route_it == conf.routes.end())
		return ;
	const std::pair<ft::string, ServerRoute>& foundDir = *route_it;
	string reqType;
	switch (request.getReqType())
	{
		case GET:
			reqType = "GET";
			break;
		case HEAD:
			reqType = "HEAD";
			break;
		case PUT:
			reqType = "PUT";
			break;
		case POST:
			reqType = "POST";
			break;
		case DELETE:
			reqType = "DELETE";
			break;
		default:
			reqType = "GET";
			break;
	}
    if (std::count(foundDir.second.limit_except.begin(),
			foundDir.second.limit_except.end(), reqType) == 0)
		throw ErrorPage(conf, "405 NOT Allowed" );
}
void ServerManager::ProcessResponse(Request &request,Response &res)
{
    envMap = request.modifyEnv(envMap);
    std::string host = request.getHost();
    const ft::string& urlx = request.getReqUrl();
	string url = urlx;
    
    if (host.empty())
        throw std::runtime_error("400");

    // Find the server that matches the host
    std::vector<Server>::iterator serv_it = findServer(
                servers.begin(), servers.end(), host);
    if (serv_it == servers.end())
       throw std::runtime_error("400");
    
    //Get the Server Configuration
	const ServerTraits& conf = (*serv_it).getConf(); 
	normalizeUrl(url);
    // Get the route for the URL
    string routeUrl = url;
    ServerRoute route = getRoute(routeUrl, conf);
    if(route.root.empty())
        route.root = conf.root;//default root
    if (conf.client_max_body_size < (request.getContLen() + request.getHeaderLength()))
         throw ErrorPage(conf, "400");
    
    // Construct the full path
    std::string path = route.root  + url.substr(routeUrl.length());
    if (!path.empty() && path[path.size() - 1] == '/')
        path.resize(path.size() - 1);
    throwIfnotAllowed(url, conf, request);
    if (redirect(route, res))
        return ;  
    
    // Handle DELETE, POST, and PUT requests
    if ((request.getReqType() == DELETE || request.getReqType() == POST || request.getReqType() == PUT) && !request.isCgi())
    {
        res.setResBody(path, request);
        return;
    }
    // Handle file or directory requests
    if (is_file(path))
    {       
        handleFileRequest(path, request, res, conf);
        return;
    }
    if (!is_dir(path))
        throw ErrorPage(conf, "404");
    // handleRequestType(request, res, path, route, conf);
    // std::map<ft::string, ServerRoute>::const_iterator route_it(
    //     conf.routes.find(url));

    // // Didn't find the dir
    // if (route_it == conf.routes.end())
    //     throw ErrorPage(conf, "404");
   
    // // Handle directory responses (index or autoindex)
    handleDirectoryResponse(route, path, request, res,conf);
    
}
void ServerManager::handleFileRequest(const std::string& path, Request& request, Response& res, const ServerTraits& conf)
{
    (void)conf;
    if (request.isCgi())
    {
        Cgi cgi;
        cgi.SetEnv(this->envMap, res, request);
        cgi.HandleCgi(res, request, conf.root, conf);
    }
    else
        res.setResBody(path, request);
}
void ServerManager::handleDirectoryResponse(ServerRoute& route, const std::string& path, 
    const Request& request, Response& res,const ServerTraits& conf)
{    // Check for index files
    for (size_t i = 0; i < route.index.size(); ++i)
    {
        std::string indexPath = route.root + route.index[i];
        if (is_file(indexPath))
        {
            res.setResBody(indexPath, request.getReqUrl());
            return;
        }
    }
    

    // Handle autoindex if enabled
    if (route.autoindex)
    {
        res.setResBody(path, request, true);
        return;
    }
    throw ErrorPage(conf,"404 Not Found");
}

void ServerManager::normalizeUrl(ft::string& url) {
    // Remove trailing slash if itif's not the root
    if (url.size() > 1 && url[url.size() - 1] == '/')
        url.resize(url.size() - 1);

    // Replace duplicate slashes with a single slash
    size_t pos;
    while ((pos = url.find("//")) != std::string::npos) {
        url.replace(pos, 2, "/");
    }

    // Handle relative paths (e.g., "../")
    // Optional: Implement logic to resolve relative paths if needed
}
static std::map<std::string, std::string> defaultErrorPages;

const std::string& getCachedErrPage(const std::string& code, const std::string& mssg)
{
    std::string key = code + ":" + mssg;

    // Check if the error page is already cached
    if (defaultErrorPages.find(key) == defaultErrorPages.end())
    {
        // Generate and cache the error page
        defaultErrorPages[key] = getErrPage(code, mssg);
    }

    return defaultErrorPages[key];
}
static void setErrorResponse(Response &res, const Request &req, const std::string& code, const std::string &mssg, const std::string& body)
{
    res.setResponseHeader(code, mssg);
    res.setErrBody(body, req);
}
void setDefaultErrPage(Response &res, const Request &req, const std::string& code, const std::string &mssg)
{
    setErrorResponse(res, req, code, mssg, getCachedErrPage(code, mssg));
}


static void setErrPage(Response &res, const Request &req, const std::string& code, const std::string &mssg, const ServerTraits& conf)
{
    res.setResponseHeader(code, mssg);
	string page;
	if (conf.error_pages.find(code) != conf.error_pages.end())
		page = conf.error_pages.find(code)->second;
	else
		page = getErrPage(code, mssg);
	
	res.setErrBody(page, req);
}
Response ServerManager::ManageRequest(const std::string& buffer)
{
    Response response;
    Request request(buffer);
    string arr[] = {"400", "403", "404", "405", "500", "504"};
	string msgArr[] = {"Bad Request", "Forbidden", "Not Found", "Method Not Allowed",
		"Internal Server Error", "Gateway Timeout"};
    try
    {
        request.parseRequest(true);
        ProcessResponse(request, response);
    }
    catch (const ErrorPage& e)
    {
        string what = e.what();

	    setErrPage(response, request, what, msgArr[(std::find(arr, arr + 6, what) - arr)], e.getConf());
    }
    catch (const std::runtime_error& e)
    {
        string what = e.what();

		setDefaultErrPage(response, request, what, msgArr[(std::find(arr, arr + 6, what) - arr)]);
    }
    catch (const std::exception& e)
    {
       std::cerr << "Error: " << e.what() << std::endl;
		response.setResponseHeader("500", "Internal Server Error");
		response.setErrBody(getErrPage("500", "Internal Server Error"), request);
    }
    return response;
}
bool ServerManager::redirect(const ServerRoute& route, Response& res)
{
	if (!route.return_.empty())
	{
		res.setResponseHeader("301", "Moved Permanently");

		// Last header
		res.appendHeader("Location: " + route.return_ + "\r\n");
		return true;
	}
	return false;
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
    std::string	first("");
	bool		flag;
	first = buff;
	Request	req(first);
	req.parseRequest();
	std::map<std::string, std::string> reqMap = req.getRequest();
    // Check if the buffer contains the complete headers
    if (buff.find("\r\n\r\n") == std::string::npos)
		return (false);
    // Return false if the buffer is empty (no data)
	if (buff.empty())
		return (false);
    	if (reqMap["Transfer-Encoding:"] == "chunked")
	{
		if (buff.find("0" CRLF CRLF) == std::string::npos)
			return (false);
		flag = true;
	}
	if ((buff.length() < (req.getContLen() + req.getHeaderLength())) && req.getReqType() == POST && !flag)
		return (false);
    return (true);
}
