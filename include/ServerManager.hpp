#pragma once

#include "Response.hpp"
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
        std::map<std::string,std::string>envMap;
        std::map<int, string> clientBuffers; // To store client data
        std::map<int, bool> isReqComplete;
       
    public:
        ServerManager(const std::vector<ServerTraits>& cnf);
        ~ServerManager();
        ServerManager(const ServerManager &src);
        ServerManager& operator=(const ServerManager &src);

        void run(char **envp);
        void parseEnv(char **rawEnv);
        std::string	strToUpper(std::string str); 
        bool partialRequest(std::string	&buff);
        Response ManageRequest(const std::string& buffer);  
        void ProcessResponse(Request& request,Response& res); 
        void normalizeUrl(string& url);   
        ServerRoute getRoute(string& url, const ServerTraits& conf);
        void throwIfnotAllowed(const string& url, const ServerTraits& conf,
            const Request& request);
        bool   redirect(const ServerRoute& route, Response& res);
        void handleFileRequest(const std::string& path, Request& request, Response& res, const ServerTraits& conf);
        void handleDirectoryResponse(ServerRoute& route, const std::string& path,
             const Request& request, Response& res,const ServerTraits& conf);
    };
void handle_exit(int sig);



