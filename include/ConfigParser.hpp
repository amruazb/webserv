/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shmuhamm <shmuhamm@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 13:50:40 by hsalah            #+#    #+#             */
/*   Updated: 2025/03/22 12:07:42 by shmuhamm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class ParserConf
{
	public:
		typedef std::vector<ft::string> Directive;
		typedef std::map<ft::string, Directive> Module;

	private:
		ft::string text;
		ft::string::iterator iter;
		std::vector< std::pair< ft::string, std::vector<ParserConf::Module> > > conf;

		static bool isModuleName(ft::string& str);
		static void removeComment(ft::string& str);
		static void replaceSpaces(ft::string& str);
		void fillRouteValue(ServerRoute& route, string& name, std::vector<string>& segments);
		void fillServerValue(ServerTraits& route, string& name, std::vector<string>& segments);

	public:
		ParserConf();
		ParserConf(ft::string& file);
		ParserConf(const ParserConf& src);
		ParserConf& operator = (const ParserConf& src);
		~ParserConf();

		std::vector<ServerTraits> parseFile();
};

void setAddress(ft::string& confAdrss, in_addr_t &address, in_port_t& port);