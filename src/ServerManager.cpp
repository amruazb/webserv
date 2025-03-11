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
                std::string response = ManageRequest(clientBuffers[pfd.fd]);

                // if (response.empty()) {
                //     response = generateErrorResponse("400", "Bad Request");
                // }

                std::cout << "📤 Sent response to FD " << pfd.fd << ": " << response << std::endl;
                send(pfd.fd, response.c_str(), response.length(), 0);

                close(pfd.fd);
                sockets.erase(sockets.begin() + i);
                clientBuffers.erase(pfd.fd);
                --i;
            }

            }
    }
}
std::string generateErrorResponse(const std::string &statusCode, const std::string& statusMessage) {
    std::ostringstream response;

    std::string body = "<html><head><title>" + statusMessage + "</title></head>"
                       "<body><h1>" + statusMessage + "</h1></body></html>";

    response << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;


    return response.str();
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
		if (((std::find((*it).getConf().server_name.begin(),
			(*it).getConf().server_name.end(), addStr) != (*it).getConf().server_name.end())
			|| (std::find((*it).getConf().server_name.begin(),
			(*it).getConf().server_name.end(), "_") != (*it).getConf().server_name.end()))
			&& (*it).getConf().listen_port == port)
			return (it);
	}
	return (it);
}
void ServerManager::ProcessResponse(Request &request)
{
    std::string host = request.getHost();

    if (host.empty()) 
        throw std::runtime_error("400"); 
    std::vector<Server>::iterator serv_it = findServer(
		servers.begin(), servers.end(), host);
}
std::string ServerManager::ManageRequest(const std::string& buffer)
{
    std::string Response;
    std::string arr[] = {"400", "403", "404", "405", "500", "504"};
    std::string msgArr[] = {"Bad Request", "Forbidden", "Not Found", "Method Not Allowed",
        "Internal Server Error", "Gateway Timeout"};

    try
    {

        Request request(buffer);
        
        ProcessResponse(request);
    }
    catch (const std::exception& e)
    {
        std::string what = e.what();
        Response = generateErrorResponse(what, msgArr[(std::find(arr, arr + 6, what) - arr)]);
    }
    return Response;
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