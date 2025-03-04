#pragma once

#include "webserv.hpp"

class Server
{
    private:
        struct sockaddr_in address; 
        int serverFd;
        int addrlen;
    public:
        Server(int portNumber);
        ~Server();
        Server(const Server& src);
	    Server& operator = (const Server& src);

        int getServerFd() const;
        struct sockaddr *getAddress() const;
        socklen_t *getAddrlen() const;
        

};