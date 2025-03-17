#pragma once
#include "webserv.hpp"

class Response
{
    private:
        std::string header;
        std::string code;
        std::string mssg;
        std::string content_type;
        int         content_len;
        std::string res_body;
        std::string res;


    public:
        Response();
        Response(const Response &src);
        Response& operator=(const Response &src);
        ~Response();
        std::string getHeader();
        void setHeader();
        void setCode(std::string code);
        void setMssg(std::string mssg);
        void setResponseHeader(std::string code, std::string mssg);
        void setErrBody(std::string body, const Request &req);
        void setResBody(std::string body, const Request &req);
        std::string getRes();
        
};