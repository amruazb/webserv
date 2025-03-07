#pragma once

#include "webserv.hpp"

class Request
{
private:
    std::string _reqUrl;
    std::map<std::string, std::string> _headers;
public:
    Request(const std::string &raRequest);
    ~Request();

    void parseRequest(const std::string &rawRequest);
    const std::string &getReqUrl() const;
    std::string getHost() const;
};