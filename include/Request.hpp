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
    CONNECT,
    UNKNOWN
};
class Request
{
private:
    std::string _reqUrl;
    std::map<std::string, std::string>      _headers;
    std::map<std::string, std::string>	    _request;
	std::map<std::string, std::string>	    _queryMap;
    e_requestType                            _type;

public:
    Request(const std::string &raRequest);
    ~Request();

    std::map<std::string, std::string>	modifyEnv(std::map<std::string, std::string> env);
    std::map<std::string, std::string>	parseUnderScore();
    void parseRequest(const std::string &rawRequest);
    e_requestType						getReqType() const;

    const std::string &getReqUrl() const;
    std::string getHost() const;
    std::string                         replaceotherChar(std::string str);
    std::string							replaceChar(std::string str);
	std::string							strToUpper(std::string str);
    e_requestType setReqType(const std::string& method) const;

    
};