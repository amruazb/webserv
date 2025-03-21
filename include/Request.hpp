#pragma once

#include "webserv.hpp"

enum e_requestType
{
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    DEFAULT
};
class Request
{
private:
		std::map<std::string, std::string>	_request;
		std::map<std::string, std::string>	_queryMap;
		e_requestType						_type;
		std::string							_buff;
		std::string							_buffCopy;
		std::string							_reqUrl;
		std::string							_host;
		std::string							_postBody;
		size_t								_contLen;
		bool								_isCgi;
		bool								_postFlag;
		bool								_isFileUpload;
		std::string							_putCode;
		std::string							_queryString;
		std::string							queryURl;
		std::string							deleteURl;
		bool								_flagEnc;
  

public:
	Request(const Request &object);
    Request();
	Request &operator=(const Request &rhs);
    Request( std::string buffer);
    ~Request();

    std::map<std::string, std::string>	modifyEnv(std::map<std::string, std::string> env);
    std::map<std::string, std::string>	parseUnderScore();
    std::map<std::string, std::string>  getRequest() const;
    void                                parseRequest(bool flag = false);
    e_requestType						getReqType() const;
    const std::string &getReqUrl() const;
    std::string getHost() const;
    std::string                         replaceotherChar(std::string str);
    std::string							replaceChar(std::string str);
	std::string							strToUpper(std::string str);
	std::string							getQueryString() const;
	std::string							getCgiUrl() const;
	std::string							getPostBody() const;
    std::string							getPutCode() const;
    std::string		                    getStrRequestType() const;
    std::string							getQueryUrl() const;
    std::string                         getDeleteURL() const;
    size_t getContLen();
    void parsePostBody();
    void                                parseQueryUrl();
    void                                headerValidation();
    void                                parseChunkedBody();
    void fileUpload();
    void setDeleteURL();
    int hexadecimalToDecimal(string hexVal);
    int getHeaderLength();
    void parseHexReqUrl();
    void setReqType(const std::string &key);
    bool        isCgiRequest(const std::string &url) const;
    bool        isCgi() const;
    bool        isWhiteSpace(std::string str1);
};