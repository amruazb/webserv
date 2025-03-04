#include "Server.hpp"

Server::Server()
{
    std::memset(&address,0,sizeof(address));
    // Create a TCP socket (SOCK_STREAM).
    serverFd = socket(AF_INET,SOCK_STREAM, 0);
    if (serverFd < 0)
		throw std::runtime_error("Socket Error");
    
    // Set the Socket to Non-Blocking Mode
    int flags = fcntl(serverFd, F_GETFL, 0);
    if (flags < 0)
        throw std::runtime_error("fcntl Error");

    if (fcntl(serverFd, F_SETFL, flags | O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl Error");

    if (fcntl(serverFd, F_SETFD, FD_CLOEXEC) < 0)
        throw std::runtime_error("fcntl Error");

    // Enable SO_REUSEADDR(Allows the same port to 
    // be reused immediately after the server rest) Option
    
    int optval = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		throw std::runtime_error("setsockopt Error");
    
    //  Bind the Socket to the Address and Port
    address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);
	address.sin_family = AF_INET;  
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("Bind Error");
    else
    if (listen(serverFd, 50) < 0)
    throw std::runtime_error("Listen Error");  
    else
    std::cout << "✅ Server is listening on port 8080\n";

}
Server::~Server()
{

}

