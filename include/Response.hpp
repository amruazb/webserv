#pragma once
#include "webserv.hpp"
#include "error.hpp"

class Request;
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
        std::map<std::string, std::string>	mimes;

    public:
        Response();
        Response(const Response &src);
        Response& operator=(const Response &src);
        ~Response();
        std::string getHeader();
		void setCgiBody(std::string body);
        void setHeader();
        void setCode(std::string code);
        void setMssg(std::string mssg);
        void setResponseHeader(std::string code, std::string mssg);
        void setErrBody(std::string body, const Request &req);
        void setResBody(std::string body, const Request &req,bool autoindex = false);
        void parseMimes();
        void appendHeader(const std::string& str);
        std::string getRes();
        
};
int	is_dir(const string path);
int is_file(const string path);