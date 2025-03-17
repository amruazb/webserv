#pragma once

#include <vector>
#include <map>
#include <ft_string.hpp>
#include <netinet/in.h>
using namespace ft;

struct ServerRoute
{
	ServerRoute();

	std::vector<string> limit_except;
	bool autoindex;
	string return_;
	std::vector<string> index;
	string root;
};


struct ServerTraits
{
    ServerTraits();

    string root;
	in_addr_t listen_address;
	in_port_t listen_port;
	std::vector<string> server_name;
	std::vector<string> cgi_extensions;
	size_t client_max_body_size;
	std::map<string, string> error_pages;
	std::map<string, ServerRoute> routes;

};