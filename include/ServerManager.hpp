#pragma once

#include "webserv.hpp"
class Server;
class Request;
class ServerManager
{
    private:
        std::vector<Server> servers;
        std::vector<struct pollfd> sockets;
        std::map<int, std::string> clientBuffers; // To store client data
        std::map<int, bool> isReqComplete;
       
    public:
        ServerManager(const std::vector<ServerTraits>& cnf);
        ~ServerManager();
        ServerManager(const ServerManager &src);
        ServerManager& operator=(const ServerManager &src);

        void run();
        bool partialRequest(std::string	&buff);
        std::string ManageRequest(const std::string& buffer);  
        void ProcessResponse(Request& request);     
};
void handle_exit(int sig);
std::string generateErrorResponse(const std::string &statusCode, const std::string &statusMessage);