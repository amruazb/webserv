#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/ip.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <dirent.h>
#include <map>
#include <string>
#include <csignal>
#include <sstream>
#include <sys/stat.h>
#include <stdexcept>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include "ft_string.hpp"
#include "ServerTraits.hpp"
#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Server.hpp"
#include <algorithm>
#include <exception>
#include "Client.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Cgi.hpp"
#include "Pages.hpp"




# define CRLF "\r\n"
#define BUFFER_SIZE 216000


