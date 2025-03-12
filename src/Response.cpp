#include "Response.hpp"

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
   header += "HTTP/1.1"+ code + " "+ mssg + "\r\n";

}
void Response::setCode(std::string code)
{
    this->code = code;
}
void Response::setMssg(std::string mssg)
{
    this->mssg = mssg;
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
    if(req.getType() == HEAD)
        res_body.clear();
    header += "Content-Type: " + content_type + "\r\n";
    header += "Content-Length: " + ft::to_string(content_len) + "; charset=utf-8""\r\n";
    // res += "Connection: close\r\n";
    header += "\r\n";
   
}