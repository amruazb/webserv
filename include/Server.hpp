#pragma once

#include "webserv.hpp"

class Server
{
    private:
        struct sockaddr_in address; 
        int serverFd;
    public:
        Server();
        ~Server();
        

};