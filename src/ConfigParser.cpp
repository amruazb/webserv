#include "../include/ConfigParser.hpp"

ServerTraits::ServerTraits()
{
	routes["/"].root = root;
	client_max_body_size = 1024;
	root = "/var/www/html";
	cgi_extensions.push_back("");
	server_name.push_back("_");
}
ServerRoute::ServerRoute()
{

}
ConfigParser::ConfigParser(const std::string& filename) : configFile_(filename)
{
    if (!configFile_.is_open()) {
        throw std::runtime_error("❌ Error: Could not open file '" + filename + "'");
    }
    if (!configFile_.good()) {
        throw std::runtime_error("❌ Error: Could not read file '" + filename + "'");
    }
}

ConfigParser::~ConfigParser()
{
    configFile_.close();
}

void ConfigParser::parseConfig()
{
    std::string line;
    if (configFile_.is_open()) {
        while (std::getline(configFile_, line))
        {
            // Remove leading and trailing whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            if (line.empty())
                continue;
            if (line[0] == '#')
                continue;
            if (line.find("server {") != std::string::npos)
            {
                parseServerBlock();
                break; // Exit the loop after parsing the server block
            }
        }
    }
}
    



void ConfigParser::parseServerBlock()
{
    std::string line;
    while (std::getline(configFile_, line))
    {
        
        // Remove leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#') 
        {
        continue;
        }
        // Handle server root
        if (line.find("root") == 0) 
        {
            size_t pos = line.find(" ");
            serverTraits_.root = line.substr(pos + 1);
            // std::cout << "debugprint--" << serverTraits_.root << "\n";
        }
        // Handle listen address
    else if (line.find("listen") == 0) 
    {
        size_t pos = line.find(" ");
        std::string listenStr = line.substr(pos + 1);
        size_t colonPos = listenStr.find(":");
        if (colonPos != std::string::npos) 
        {
            std::string address = listenStr.substr(0, colonPos);
            if (address == "localhost") {
                serverTraits_.listen_address = inet_addr("127.0.0.1");  // Explicitly set localhost to 127.0.0.1
            } else {
                serverTraits_.listen_address = inet_addr(address.c_str());
            }
            serverTraits_.listen_port = atoi(listenStr.substr(colonPos + 1).c_str());
        } else 
        {
            serverTraits_.listen_address = inet_addr("0.0.0.0"); // default to 0.0.0.0 if no address is provided
            serverTraits_.listen_port = atoi(listenStr.c_str());
        }
    }
        // Handle server name
    else if (line.find("server_name") == 0) 
    {
    size_t pos = line.find(" ");
    std::string server_nameStr = line.substr(pos + 1);
    std::cout <<"DEBUGGING" <<server_nameStr << std::endl;
    size_t spacePos = server_nameStr.find(" ");
    if (spacePos != std::string::npos) 
    {
        while (spacePos != std::string::npos) 
        {
            serverTraits_.server_name.push_back(server_nameStr.substr(0, spacePos));
            server_nameStr = server_nameStr.substr(spacePos + 1);
            spacePos = server_nameStr.find(" ");
        }
        serverTraits_.server_name.push_back(server_nameStr);
    } 
    else 
    {
        std::cout <<"DEBUGGING" <<server_nameStr << std::endl;
        serverTraits_.server_name.push_back(server_nameStr);
        std::cout <<"DEBUGGING" <<serverTraits_.server_name[1] << std::endl;
    }
}

        // Handle client max body size
        else if (line.find("client_max_body_size") == 0) {
            size_t pos = line.find(" ");
            serverTraits_.client_max_body_size = atoi(line.substr(pos + 1).c_str());
        }
        // End of server block
        else if (line.find("}") == 0) {
            break;
        }
    }
    
}

ServerTraits ConfigParser::getServerTraits() const 
{
    return serverTraits_;
}

int main() {
  ConfigParser parser("test_config.config");
  parser.parseConfig();

  // Verify server settings
  std::cout << "Server root: " << parser.getServerTraits().root << std::endl;
  std::cout << "Listen address: " << parser.getServerTraits().listen_address << std::endl;
  std::cout << "Listen port: " << parser.getServerTraits().listen_port << std::endl;
  std::cout << "Server name: " << parser.getServerTraits().server_name[0] << std::endl;
  std::cout << "Client max body size: " << parser.getServerTraits().client_max_body_size << std::endl;

  return 0;


}

