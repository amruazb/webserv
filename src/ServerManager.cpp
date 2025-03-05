#include "ServerManager.hpp"

bool isExit = false;
// 🚀 FIXME:: Ask teammate about storing the info in config
//          ip address and port is needed to initialize the server
//          Discuss how address and ip address (obtained from server.config)are stored
//          needs to change the server constructor accordingly

ServerManager::ServerManager(const std::vector<int>& ports)
{
    try {
        for (size_t i = 0; i < ports.size(); ++i) {
            servers.push_back(Server(ports[i])); // Create servers

            // Add server sockets to poll list
            struct pollfd pfd;
            pfd.fd = servers.back().getServerFd();
            pfd.events = POLLIN;
            pfd.revents = 0;

            sockets.push_back(pfd);  // Now push the initialized struct into the vector

            std::cout << "✅ Server is listening on port " << ports[i] << std::endl;
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
                std::string response = "HTTP/1.1 200 NOT FOUND\r\nContent-Length: 13\r\n\r\nHello, World!";
                send(pfd.fd, response.c_str(), response.length(), 0);

                std::cout << "📤 Sent response to FD " << pfd.fd << std::endl;
                // std::string res = ManageRequest(clientBuffers[pfd.fd]);
				// send(pfd.fd, res.c_str(),
				// 	sizeof(res), 0);
				
				// isReqComplete[pfd.fd] = false;
                // Close connection after response
                close(pfd.fd);
                sockets.erase(sockets.begin() + i);
                clientBuffers.erase(pfd.fd);
                --i;
            }

            }
    }
}
// std::string ServerManager::ManageRequest(const std::string& buffer)
// {



// }
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