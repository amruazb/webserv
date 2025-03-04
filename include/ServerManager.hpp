#pragma once

#include "webserv.hpp"
class Server;

class ServerManager
{
    private:
        std::vector<Server> servers;
        std::vector<struct pollfd> sockets;
        std::map<int, std::string> clientBuffers; // To store client data
    public:
        ServerManager(std::vector<int> ports);
        ~ServerManager();

        void run();



};
void handle_exit(int sig);