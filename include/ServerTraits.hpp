/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerTraits.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsalah <hsalah@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 09:52:17 by hsalah            #+#    #+#             */
/*   Updated: 2025/03/20 09:52:28 by hsalah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <map>
#include <ft_string.hpp>
#include <netinet/in.h>

using namespace ft;

/**
 * @brief Struct to store the info of a server route, eg. [location /]
 * 
 * @param limit_except A vector to store the allowed http methods.
 * 	eg. {"GET", "POST", "DELETE"}
 * 
 * @param autoindex A bool to tell if a directory should be listed or not
 * 	when a directory link is requested.
 * 
 * @param return_ A string that stores a redirection path. note: called "return" in conf file.
 */
struct ServerRoute
{
	ServerRoute();

	std::vector<string> limit_except;
	bool autoindex;
	string return_;
	std::vector<string> index;
	string root;
};

/**
 * @param root A string that stores the path to the server root.
 * 
 * @param index A vector of file names, relative to path.
 * 
 * @param listen_address A 32 bit int that stores the address to listen to.
 * 
 * @param listen_port An unsigned 16 bit int that stores the port to listen to.
 * 
 * @param server_name A vector to store the server names.
 * 
 * @param client_max_body_size A size_t that stores the amount of bytes
 * 	that can be sent in the response body.
 * 
 * @param routes A vector of ServerRoute struct, that stores the routes info.
 * 
 * @param error_pages A map that stores the error pages.
 * 
 * @param cgi_extensions A vector that stores the allowed cgi extensions.
 * 
*/
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
