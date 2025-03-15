#include "ConfigParser.hpp"



ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(ft::string& input) 
{
    std::ifstream file(input.c_str());  // C++98-compliant way to open a file
    if (!file) {
        throw std::runtime_error("Error: Unable to open config file");
    }

    std::stringstream textStream;
    textStream << file.rdbuf();  // Read the entire file
    file.close();  // Close the file explicitly (good practice)

    text = textStream.str();  // Store content in the member variable

    if (text.empty()) {
        throw std::runtime_error("Error: Config file is empty");
    }

    iter = text.begin();  // Initialize iterator
}

ConfigParser::~ConfigParser()
{
    
}
ConfigParser::ConfigParser(const ConfigParser& src)
{
	if (this == &src)
		return ;
	*this = src;
}

ConfigParser& ConfigParser::operator = (const ConfigParser& src)
{
	if (this == &src)
		return *this;
	text = src.text;
	iter = src.iter;
	return *this;
}

// Replaces all spaces with ' ' except '\\n'
void ConfigParser::replaceSpaces(std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (std::isspace(str[i]) && str[i] != '\n')
			str[i] = ' ';
	}
}
// Sets the end of the str to be ';' exclusive
void ConfigParser::removeComment(std::string& str)
{
	str = str.substr(0, str.find(';'));
}
bool ConfigParser::isModuleName(ft::string& str)
{
	return (*(str.begin()) == '[' && *(str.end() - 1) == ']');
}
void replaceSpacesWithPercents(ft::string& str)
{
	str = str.replace_all(" ", "%20");
}
void ConfigParser::fillRouteValue(ServerRoute& route, string& name,
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

void ConfigParser::fillServerValue(ServerTraits& server, string& name, std::vector<string>& segments)
{
	if (name == "listen")
	{
		std::cout << "i am in listen " << std::endl;
		if (segments.size() > 1)
			throw std::runtime_error("Parse Error: 'listen' should have 1 value");
		setAddress(segments.front(), server.listen_address, server.listen_port);
		std::cout << "listening port" << server.listen_port<<std::endl;
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

std::vector<ServerTraits> ConfigParser::parseConfig()
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

			// First element should always be a Module and there should be a name at the start.
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
    




void setAddress(ft::string& confAdrss, in_addr_t &address, in_port_t& port)
{
    std::vector<ft::string> vec = confAdrss.split(':');
    if (vec.size() != 1 && vec.size() != 2)
        throw std::runtime_error("400"); // Bad Address: Invalid format

    // Only port provided
    if (vec.size() == 1)
    {
		address = htonl(INADDR_ANY);
        port = ft::from_string<in_port_t>(vec.at(0));
		std::cout << "port is" << port << std::endl;
		if (port <= 0)
			throw std::runtime_error("400"); // Bad Address: Invalid format
        port = htons(port);
        return;
    }

    // Handling address
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

ServerTraits ConfigParser::getServerTraits() const 
{
    return serverTraits_;
}

// int main() {
//    ConfigParser parser("test_config.config");
//   parser.parseConfig();

//   // Verify server settings
//   std::cout << "Server root: " << parser.getServerTraits().root << std::endl;
//   std::cout << "Listen address: " << parser.getServerTraits().listen_address << std::endl;
//   std::cout << "Listen port: " << parser.getServerTraits().listen_port << std::endl;
//   std::cout << "Server name: " << parser.getServerTraits().server_name[0] << std::endl;
//   std::cout << "Client max body size: " << parser.getServerTraits().client_max_body_size << std::endl;

//   return 0;


// }

