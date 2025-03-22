#include "Response.hpp"
#include <sys/stat.h>


Response::Response()
{
    header = "HTTP/1.1";
    code = "200";
    mssg = "OK";
    content_type = "text/html";
    content_len = 0;
    res_body = "";
    res = "";
    setResponseHeader(code, mssg);
    parseMimes();
}
void	Response::setCgiBody(std::string body)
{
	this->res_body.clear();
	this->res_body = body;
	this->content_len = res_body.length();
	this->header += "Content-Type: " + content_type + "; charset=utf-8" CRLF
					"Content-Length: " + ft::to_string(this->content_len) + CRLF
					CRLF;
}
Response::Response(const Response &src)
{
    if (this == &src)
        return ;
    *this = src;
}

Response& Response::operator=(const Response &src)
{
    if (this == &src)
        return *this;
    header = src.header;
    code = src.code;
    mssg = src.mssg;
    content_type = src.content_type;
    content_len = src.content_len;
    res_body = src.res_body;
    res = src.res;
    return *this;
}
void Response::setResponseHeader(std::string code, std::string mssg)
{
    setCode(code);
    setMssg(mssg);
    setHeader();
}
void Response::setHeader()
{
   header.clear();
   header += "HTTP/1.1 " +code + " "+ mssg + "\r\n";

}
void Response::setCode(std::string code)
{
    this->code = code;
}
void Response::setMssg(std::string msg)
{
    mssg = msg;
}
void Response::appendHeader(const std::string& str)
{
	header += str + "\r\n";
}
Response::~Response()
{
}
std::string Response::getRes()
{
    res.clear();
    res = header + res_body;
    return res;
}
std::string Response::getHeader()
{
    return header;
}
void Response::setErrBody(std::string body, const Request &req)
{
    res_body.clear();
    res_body = body;
    content_len = res_body.length();
    if(req.getReqType() == HEAD)
        res_body.clear();
    header += "Content-Type: " + content_type + "\r\n";
    header += "Content-Length: " + ft::to_string(content_len) + "; charset=utf-8""\r\n";
    // res += "Connection: close\r\n";
    header += "\r\n";
   
}



void Response::setResBody(std::string path, const Request &req,bool autoindex)
{
    std::string body;
    std::string type;
    size_t pos;

    // Handle GET, HEAD, DELETE requests
    if (req.getReqType() != POST && req.getReqType() != PUT) 
    {
        if (is_dir(path.c_str())) 
        {
            if (autoindex || req.getReqType() == DELETE) 
            {
                std::cout << "url is" << req.getReqUrl() << std::endl;
                std::cout<< "path is" << path << std::endl;
                body = dirList(path, req.getReqUrl());
            }
            else 
                throw ("404");
        } 
        else if (is_file(path.c_str())) 
            body = ft::file_to_string(path);
        else 
            throw ("404");
    }
    // Determine MIME type
    if ((pos = path.find_last_of('.')) != std::string::npos) 
    {
        type = path.substr(pos);
        std::map<std::string, std::string>::iterator it = this->mimes.find(type);
        if (it != this->mimes.end()) 
            content_type = it->second;     
    }
    // Set response body and content length
    res_body.clear();
    res_body = body;
    content_len = res_body.length();
    // Handle POST and PUT requests
    if (req.getReqType() == POST || req.getReqType() == PUT) {
            std::cout << "----------------------afasdas-" << std::endl;
        std::cout << req.getReqType() << std::endl;
        std::cout << req.getPutCode() << std::endl;
            std::cout << "----------------------afasdas-" << std::endl;

        if (req.getReqType() == POST) {
            this->setResponseHeader("200", "OK");
        }
        if (req.getReqType() == PUT)
        {
            this->setResponseHeader("201", "Created");
        }
        this->res_body.clear();
    }
      // Handle HEAD requests
    if (req.getReqType() == HEAD) 
        this->res_body.clear();
    
    // Handle DELETE requests
    if (req.getReqType() == DELETE) 
    {
        std::cout << req.getDeleteURL() << std::endl;
        // if (remove(req.getDeleteURL().c_str()) != 0)
        //     throw std::runtime_error("Failed to delete file: " + req.getDeleteURL());
        struct stat buffer;
        if (stat(req.getDeleteURL().c_str(), &buffer) == 0) {
            if (remove(req.getDeleteURL().c_str()) != 0)
                throw std::runtime_error("Failed to delete file: " + req.getDeleteURL());
        } else {
            std::cerr << "File not found: " << req.getDeleteURL() << std::endl;
        }

    }

    // Build response header
    std::cout << "------------------------------" << std::endl;
    std::cout << header << std::endl;
    std::cout << "------------------------------" << std::endl;
    header += "Content-Type: " + content_type + "; charset=utf-8"
                                                "\r\n"
                                                "Content-Length: " +
              ft::to_string(this->content_len) + "\r\n"
                                                 "\r\n";
}
void	Response::parseMimes()
{
	std::ifstream	mimieFile("mimes.txt");
	std::string		line;
	
	if (mimieFile.fail())
	{
		mimieFile.close();
	}
	getline(mimieFile, line);
	while (!(mimieFile.eof()))
	{
		std::stringstream	str(line);
		std::string			ext, type;
		getline(str, ext, ' ');
		getline(str, type);
		this->mimes[ext] = type;
		getline(mimieFile, line);
	}
	mimieFile.close();
}

int	is_dir(const string path)
{
	struct stat	statbuf;

	if (stat(path.c_str(), &statbuf) != 0)
		return (0);
	return (S_ISDIR(statbuf.st_mode));
}

int is_file(const string path)
{
	struct stat	statbuf;

	if (stat(path.c_str(), &statbuf) != 0)
		return (0);
	return (S_ISREG(statbuf.st_mode));
}
