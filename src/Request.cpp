#include "Request.hpp"

Request::Request(const std::string& rawRequest)
{
   
  parseRequest(rawRequest);  
}
Request::~Request()
{

}
e_requestType Request::setReqType(const std::string& method) const
{
    if (method == "GET")
        return GET;
    else if (method == "POST")
        return POST;
    else if (method == "PUT")
        return PUT;
    else if (method == "DELETE")
        return DELETE;
    else if (method == "HEAD")
        return HEAD;
    else if (method == "OPTIONS")
        return OPTIONS;
    else
        return UNKNOWN; // For unsupported or unknown methods
}
void Request::parseRequest(const std::string& rawRequest)
{
    std::istringstream stream(rawRequest);
    std::string line;
    if (std::getline(stream, line))
    {
        std::istringstream lineStream(line);
        std::string method, url, version;
        lineStream >> method >> url >> version;
        _reqUrl = url;

        // Determine and store the request type
        _type = setReqType(method);

        // Check if the request is for a CGI script
        _isCgi = isCgiRequest(url);
    }
    // Parse headers
        while (std::getline(stream, line) && line != "\r") 
        {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 2);
                _headers[key] = value;
            }
        }
}
const std::string& Request::getReqUrl() const 
{
    return _reqUrl;
}
std::string Request::getHost() const 
{
    std::map<std::string, std::string>::const_iterator it = _headers.find("Host");
    if (it != _headers.end()) {
        return ft::string(it->second).replace_all(" ", "%20");
    }
    return "";
}
std::string	Request::strToUpper(std::string str)
{
    for(size_t i = 0; i < str.length(); i++) {
        str[i] = toupper(str[i]);
    }
	return (str);
}

std::string Request::replaceChar(std::string str)
{
	for (size_t pos = str.find('-'); pos != std::string::npos; pos = str.find('-'))
	{
		str.replace(pos, 1, "_");
	}
	return(str);
}

std::map<std::string, std::string>	Request::parseUnderScore()
{
	std::map<std::string, std::string> mapCopy = _request;
	std::map<std::string, std::string> mapC;
	std::map<std::string, std::string>::iterator it;
	for (it = mapCopy.begin(); it != mapCopy.end(); ++it)
	{
		std::string key = replaceChar(it->first);
		key = replaceotherChar(it->first);
		key = strToUpper(key);
        key = "HTTP_" + key;
		key = ft::string(key).replace_all("-", "_");
		mapC[key] = it->second;
	}
	return (mapC);
}
std::map<std::string, std::string> Request::modifyEnv(std::map<std::string, std::string> env)
{
	std::map<std::string, std::string> updatedReq = parseUnderScore();
	std::map<std::string, std::string>::iterator it1;
	for (it1 = updatedReq.begin(); it1 != updatedReq.end(); ++it1)
	{

		env[it1->first] = it1->second;
	}
	for (it1 = _queryMap.begin(); it1 != _queryMap.end(); ++it1)
	{
		env[it1->first] = it1->second;
	}
	return env;
}
std::string Request::getCgiUrl() const{

    std::string path = this->getReqUrl();
    std::string ret;

    if (path.find_first_of('?') != std::string::npos)
    {
        ret = path.substr(0, path.find_first_of('?'));
        return (ret);
    }
    return(path);
}
std::string Request::replaceotherChar(std::string str)
{
	for (size_t pos = str.find(':'); pos != std::string::npos; pos = str.find(':'))
	{
		str.replace(pos, 1, "");
	}
	return(str);
}
bool Request::isCgiRequest(const std::string& url) const
{
    // List of CGI extensions
    static  std::vector<std::string> cgiExtensions ;
     // Populate the vector only once
     if (cgiExtensions.empty()) {
        cgiExtensions.push_back(".cgi");
        cgiExtensions.push_back(".pl");
        cgiExtensions.push_back(".php");
    }

    // Check if the URL ends with any of the CGI extensions
    for (size_t i = 0; i < cgiExtensions.size(); ++i)
    {
        if (url.size() >= cgiExtensions[i].size() &&
            url.compare(url.size() - cgiExtensions[i].size(), cgiExtensions[i].size(), cgiExtensions[i]) == 0)
        {
            return true;
        }
    }

    // Check if the URL contains "/cgi-bin/"
    if (url.find("/cgi-bin/") != std::string::npos)
    {
        return true;
    }

    return false;
}
e_requestType Request::getReqType() const
{
    return _type;
}
bool Request::isCgi() const
{
    return _isCgi;
}

// int main() {
//     try {
//         // Example HTTP request
//         std::string rawRequest =
//             "GET /shamna/script HTTP/1.1\r\n"
//             "Host: www.example.com\r\n"
//             "User-Agent: Mozilla/5.0\r\n"
//             "Accept: text/html\r\n"
//             "\r\n";

//         Request request(rawRequest);

//         // Check if the request is for a CGI script
//         if (request.isCgi()) {
//             std::cout << "This is a CGI request." << std::endl;
//         } else {
//             std::cout << "This is not a CGI request." << std::endl;
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }

//     return 0;
// }