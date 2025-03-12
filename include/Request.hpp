#pragma once

#include "webserv.hpp"

enum e_requestType
{
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    OPTIONS,
    TRACE,
    CONNECT
};
class Request
{
private:
    std::string _reqUrl;
    std::map<std::string, std::string> _headers;
    e_requestType _type;
public:
    Request(const std::string &raRequest);
    ~Request();

    void parseRequest(const std::string &rawRequest);
    const std::string &getReqUrl() const;
    std::string getHost() const;
    e_requestType getType() const;
};