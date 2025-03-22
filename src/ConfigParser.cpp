/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsalah <hsalah@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 11:50:13 by hsalah            #+#    #+#             */
/*   Updated: 2025/03/20 09:16:55 by hsalah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ParserConf::ParserConf() {} // constructeur , init , il cree un objet vide no param. 

// replaces all spaces with ' ' except '\\n'
void ParserConf::replaceSpaces(ft::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (std::isspace(str[i]) && str[i] != '\n')
			str[i] = ' ';
	}
}

void replaceSpacesWithPercents(ft::string& str)
{
	str = str.replace_all(" ", "%20");
}

// Sets the end of the str to be ';' exclusive
void ParserConf::removeComment(ft::string& str)
{
	str = str.substr(0, str.find(';'));
}

ParserConf::ParserConf(ft::string& input)
{
	std::ifstream file(input.c_str());
	if (file.fail())
		throw std::runtime_error("Error with config file");
	std::stringstream textStream;
	textStream << file.rdbuf();
	file.close();
	this->text = textStream.str();
	iter = this->text.begin();
}

ParserConf::ParserConf(const ParserConf& src)
{
	if (this == &src)
		return ;
	*this = src;
}

ParserConf& ParserConf::operator = (const ParserConf& src)
{
	if (this == &src)
		return *this;
	text = src.text;
	iter = src.iter;
	return *this;
}

ParserConf::~ParserConf() {}

bool ParserConf::isModuleName(ft::string& str)
{
	return (*(str.begin()) == '[' && *(str.end() - 1) == ']');
}

void ParserConf::fillRouteValue(ServerRoute& route, string& name,
	std::vector<string>& segments)
{
	if (name == "limit_except")
		route.limit_except = segments;
	else if (name == "autoindex")
	{
		if (segments.size() > 1)
			throw std::runtime_error("Parse Error: 'autoindex' should have 1 value");
		if (segments.front() == "true")
			route.autoindex = true;
		else if (segments.front() == "false")
			route.autoindex = false;
		else
			throw std::runtime_error("Parse Error: could not parse 'autoindex'");
	}
	else if (name == "root")
	{
		if (segments.size() > 1)
			throw std::runtime_error("Parse Error: 'root' should have 1 value");
		route.root = segments.front();
		if ((route.root.length() > 1) && ((*(route.root.end() - 1)) == '/'))
			route.root.erase(route.root.end() - 1);
	}
	else if (name == "index")
	{
		route.index = segments;
	}
	else if (name == "return")
	{
		if (segments.size() > 1)
			throw std::runtime_error("Parse Error: 'return' should have 1 value");
		route.return_ = segments.front();
	}
	else
		throw std::runtime_error((string("Parse Error: Unknown type {") + name + string("}")).c_str());
}

void ParserConf::fillServerValue(ServerTraits& server, string& name, std::vector<string>& segments)
{
	if (name == "listen")
	{
		if (segments.size() > 1)
			throw std::runtime_error("Parse Error: 'listen' should have 1 value");
		setAddress(segments.front(), server.listen_address, server.listen_port);
	}
	else if (name == "root")
	{
		if (segments.size() > 1)
			throw std::runtime_error("Parse Error: 'root' should have 1 value");
		server.root = segments.front();
	}
	else if (name == "server_name")
	{
		server.server_name = segments;
	}
	else if (name == "client_max_body_size")
	{
		if (segments.size() > 1)
			throw std::runtime_error("Parse Error: 'client_max_body_size' should have 1 value");
		try {
			server.client_max_body_size = ft::from_string<size_t>(segments.front()); // Throws
		} catch (std::exception& e) {
			throw std::runtime_error("Parse Error: could not parse 'client_max_body_size'");
		}
	}
	else if (name == "error_page")
	{
		if (segments.size() != 2)
			throw std::runtime_error("Parse Error: 'error_page' should have 1 or 2 values");
		try {
			server.error_pages[segments.front()] = ft::file_to_string(segments.back()); // Throws
		} catch (std::exception& e) {
			throw std::runtime_error("Parse Error: could not parse 'error_page'");
		}
	}
	else if (name == "cgi_extensions")
	{
		server.cgi_extensions = segments;
	}
	else
	{
		throw std::runtime_error((string("Parse Error: Unknown type {") + name + string("}")).c_str());
	}
}

/**
 * TO DO HERE :
 * - Replace all space chars (except '\\n') with '\ ' (space).
 * - Remove everything after ';' (comments).
 * - Make your exceptions.
*/
std::vector<ServerTraits> ParserConf::parseFile()
{
	std::vector<ServerTraits> file;
	ft::string lastModlName;
	bool isRoute;

	std::vector<ft::string> lines = text.split('\n');

	for (std::vector<ft::string>::iterator it = lines.begin();
		it < lines.end(); ++it)
	{
		ft::string& line = *it;
		removeComment(line);
		replaceSpaces(line);
		line = line.strip(' ');

		if (isModuleName(line))
		{
			lastModlName = line.substr(1, line.length() - 2);
			std::vector<string> modlSplit = lastModlName.split(' ');
			if ((modlSplit.size() == 1) && lastModlName == "server")
			{
				file.push_back(ServerTraits());
				isRoute = false;
			}
			else if ((modlSplit.size() == 2) && modlSplit.front() == "location")
			{
				if (modlSplit.back()[0] != '/')
					throw std::runtime_error("Parse Error: Bad module name");

				file.back().routes[modlSplit.back().rstrip('/')] = ServerRoute();
				isRoute = true;
			}
			else
				throw std::runtime_error("Parse Error: Bad module name");
		}
		else
		{
			Directive segments = line.split(' ');

			if (segments.empty())
				continue ;

			// first element should always be a module and there should be a name at the start.
			if (file.empty() || segments.size() == 1)
				throw std::runtime_error("Parse Error: Bad module name");

			ft::string name = segments.front();
			segments.erase(segments.begin());

			std::for_each(segments.begin(), segments.end(),
				replaceSpacesWithPercents);

			if (isRoute)
				fillRouteValue(file.back().routes[
					lastModlName.split(' ').back()],
					name, segments);
			else
				fillServerValue(file.back(), name, segments);
		}
	}
	if (file.empty())
		throw std::runtime_error("Parse Error: No server found in the configuration file");
	return (file);
}

/**
 * TO DO HERE :
 * https://nginx.org/en/docs/http/ngx_http_core_module.html#listen
 * listen address[:port]
 * listen port
 * listen unix:path
 * Default:	listen *:80 | *:8000;
 * 
 * https://stackoverflow.com/questions/15673846
*/
void setAddress(ft::string& confAdrss, in_addr_t &address, in_port_t& port)
{
	std::vector<ft::string> vec = confAdrss.split(':');
	if (vec.size() != 1 && vec.size() != 2)
		throw std::runtime_error("400"); // bad address: Invalid format

	// only port provided
	if (vec.size() == 1)
	{
		address = htonl(INADDR_ANY);
		port = ft::from_string<in_port_t>(vec.at(0));
		if (port <= 0)
			throw std::runtime_error("400"); // bad address: Invalid format
		port = htons(port);
		return;
	}

	// handling address
	if (vec.at(0) != "*")
	{
		std::vector<ft::string> segments = vec.at(0).split('.');
		if (segments.size() != 4)
			throw std::runtime_error("400");

		in_addr_t adrs_int = (ft::from_string<in_addr_t>(segments.at(0)) << 24)
							| (ft::from_string<in_addr_t>(segments.at(1)) << 16)
							| (ft::from_string<in_addr_t>(segments.at(2)) << 8)
							| ft::from_string<in_addr_t>(segments.at(3));
		address = htonl(adrs_int);
	}

	// Handling port
	port = ft::from_string<in_port_t>(vec.at(1));
	port = htons(port);
}
