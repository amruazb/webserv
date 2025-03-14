#pragma once

#include "webserv.hpp"
class Server;
class Request;
class Response;
class Client;
class ServerManager
{
    private:
        std::vector<Server> servers;
        std::vector<Client> clients;
        std::vector<struct pollfd> sockets;
        std::map<int, string> clientBuffers; // To store client data
        std::map<int, bool> isReqComplete;
       
    public:
        ServerManager(const std::vector<ServerTraits>& cnf);
        ~ServerManager();
        ServerManager(const ServerManager &src);
        ServerManager& operator=(const ServerManager &src);

        void run();
        bool partialRequest(std::string	&buff);
        Response ManageRequest(const std::string& buffer);  
        void ProcessResponse(Request& request); 
        void normalizeUrl(string& url);   
        ServerRoute getRoute(string& url, const ServerTraits& conf);
};
void handle_exit(int sig);
std::string generateErrorResponse(const std::string &statusCode, const std::string &statusMessage);

class ErrorPage : public std::exception
{
    private:
        const ServerTraits& conf;
        const std::string msg;

    public:
        ErrorPage(const ServerTraits& conf,std::string msg) : conf(conf),msg(msg) {}
        virtual const char* what() const throw() { return msg.c_str(); }
        const ServerTraits& getConf() const { return conf; }
        virtual ~ErrorPage() throw() {}
};