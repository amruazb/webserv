/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsalah <hsalah@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 11:00:51 by hsalah            #+#    #+#             */
/*   Updated: 2025/03/20 09:19:27 by hsalah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv, char **envp)
{
	if (argc > 2)
		return (1);
	try
	{
		ft::string file;

		// Parsing - configuration file
		if (argc == 2)
			file = argv[1];
		else
			file = "conf.ini";
		ParserConf parser(file);
		std::vector<ServerTraits> conf = parser.parseFile(); // recup config du server 
		if (conf.empty()) // verif
			throw std::runtime_error("No server found in the configuration file");

		// Starting the server here
		ServerManager serverManager(conf);
		serverManager.run(envp);
		std::cerr << "EXITED" << std::endl;
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "parse error\n";
		std::cerr << e.what() << '\n';
	}
	catch (const std::exception& e)
	{
		std::cerr << "normal error\n";
		std::cerr << e.what() << '\n';
	}
    return (0);
}
