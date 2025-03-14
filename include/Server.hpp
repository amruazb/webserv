#pragma once

#include "webserv.hpp"
#include "Client.hpp"
class Server
{
    private:
        struct sockaddr_in address; 
        int serverFd;
        int addrlen;
        ServerTraits conf;
        // There shouldn't be a default constructor
	    Server();
    public:
        Server(const ServerTraits& cnf);
        ~Server();
        Server(const Server& src);
	    Server& operator = (const Server& src);

        int getServerFd() const;
        struct sockaddr *getAddress() const;
        socklen_t *getAddrlen() const;
        const ServerTraits& getConf() const;
        Client acceptNewClient();
};
