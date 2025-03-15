#include "Server.hpp"


Server::Server(const ServerTraits& cnf) : conf(cnf)
{
    std::memset(&address,0,sizeof(address));
    address.sin_addr.s_addr = conf.listen_address;
    address.sin_port = conf.listen_port;
    address.sin_family = AF_INET;
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
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("Bind Error");
  
    if (listen(serverFd, 50) < 0)
    throw std::runtime_error("Listen Error");  
    

}
Server::~Server()
{

}
Server::Server(const Server& src)
{
	if (this == &src)
		return ;
	*this = src;
	conf = src.conf;
}

Server& Server::operator = (const Server& src)
{
	if (this == &src)
		return *this;
	this->address = src.address;
	this->addrlen = src.addrlen;
	this->serverFd = src.serverFd;
	this->conf = src.conf;
	return *this;
}
Client Server::acceptNewClient()
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddrlen = sizeof(clientAddress);
    int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &clientAddrlen);

    if (clientFd < 0)
        throw std::runtime_error("Accept Error");

    return Client(clientFd); // Create and return Client object
}


int Server::getServerFd() const { return serverFd; }

struct sockaddr *Server::getAddress() const { return  ((struct sockaddr *)&address); }

socklen_t *Server::getAddrlen() const { return ((socklen_t*)&addrlen); }

const ServerTraits& Server::getConf() const { return (conf); }


