#pragma once
#include "webserv.hpp"

class Client 
{
    private:
        int fd_;
       
    public:
        Client(int fd);
        ~Client();
        int getFd();
        void sendResponse();
        void close_client();
        void addPoll(struct pollfd& pfd);
        int readData(string &buff);
};
Client *getClientByFd(std::vector<Client> &clients, int fd);
void removeClientByFd(std::vector<Client> &clients, int fd);
void printClients(std::vector<Client> &clients);
