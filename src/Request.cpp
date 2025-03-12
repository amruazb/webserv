#include "Request.hpp"

Request::Request(const std::string& rawRequest)
{
   
  parseRequest(rawRequest);  
}
Request::~Request()
{

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
e_requestType Request::getType() const
{
    return _type;
}

// int main() {
//     try {
//         // Example HTTP request
//         std::string rawRequest =
//             "GET /index.html HTTP/1.1\r\n"
//             "Host: www.example.com\r\n"
//             "User-Agent: Mozilla/5.0\r\n"
//             "Accept: text/html\r\n"
//             "\r\n";

//         Request request(rawRequest);

//         const std::string& url = request.getReqUrl();
//         std::string host = request.getHost();

//         if (host.empty()) {
//             throw std::runtime_error("400 Bad Request: Missing Host header");
//         }

//         std::cout << "Request URL: " << url << std::endl;
//         std::cout << "Host: " << host << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }

//     return 0;
// }