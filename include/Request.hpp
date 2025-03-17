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
    std::map<std::string, std::string>	_request;
	std::map<std::string, std::string>	_queryMap;

public:
    Request(const std::string &raRequest);
    ~Request();

    std::map<std::string, std::string>	modifyEnv(std::map<std::string, std::string> env);
    std::map<std::string, std::string>	parseUnderScore();
    void parseRequest(const std::string &rawRequest);
    const std::string &getReqUrl() const;
    std::string getHost() const;
    e_requestType getType() const;
    std::string                         replaceotherChar(std::string str);
    std::string							replaceChar(std::string str);
	std::string							strToUpper(std::string str);

    
};