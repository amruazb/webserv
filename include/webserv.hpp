#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>
#include <string>
#include <csignal>
#include <sstream>
#include <stdexcept>
#include "Request.hpp"
#include <fstream>
#include <arpa/inet.h>
#include "ft_string.hpp"
#include "ServerTraits.hpp"
#include "ConfigParser.hpp"
#include "ServerManager.hpp"
#include "Server.hpp"
#include <algorithm>
#include <exception>
#include "Server.hpp"
#include "Response.hpp"




#define BUFFER_SIZE 216000


