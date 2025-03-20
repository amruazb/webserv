/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsalah <hsalah@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 13:52:46 by hsalah            #+#    #+#             */
/*   Updated: 2025/03/20 09:16:37 by hsalah           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class Server
{

private:
	struct sockaddr_in address;
	std::list<int> clients;
	int addrlen;
	int serverFd;
	ServerTraits conf;

	// there shouldn't be a default constructor
	Server();

public:

	Server(const ServerTraits& cnf);
	Server(const Server& src);
	Server& operator = (const Server& src);
	~Server();

	int getServerFd() const;
	struct sockaddr *getAddress() const;
	socklen_t *getAddrlen() const;
	const ServerTraits& getConf() const;
};
