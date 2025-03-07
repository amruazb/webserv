#pragma once
#include "webserv.hpp"
struct ServerRoute
{
	ServerRoute();

	std::vector<std::string> limit_except;
	bool autoindex;
	std::string return_;
	std::vector<std::string> index;
	std::string root;
};


struct ServerTraits
{
    ServerTraits();

    std::string root;
    in_addr_t listen_address;
    in_port_t listen_port;
    std::vector<std::string> server_name;
    std::vector<std::string> cgi_extensions;
    size_t client_max_body_size;
    std::map<std::string, std::string> error_pages;
    std::map<std::string, ServerRoute> routes;
};


class ConfigParser {
private:
    
    std::ifstream configFile_;
    ServerTraits serverTraits_;
    std::map<std::string, ServerRoute> routes_;
public:
    ConfigParser(const std::string& filename);
    ~ConfigParser();

    void parseConfig();
    void parseServerBlock();
    ServerTraits getServerTraits() const;
    

};