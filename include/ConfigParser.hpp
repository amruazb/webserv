#pragma once
#include "webserv.hpp"



class ConfigParser {
private:
    ft::string text;
    ft::string::iterator iter;
    std::ifstream configFile_;
    ServerTraits serverTraits_;
    std::map<std::string, ServerRoute> routes_;
public:
    typedef std::vector<ft::string> Directive;
    typedef std::map<ft::string, Directive> Module;

    ConfigParser();
    ConfigParser(ft::string& input);
    ~ConfigParser();
    ConfigParser(const ConfigParser& src);
	ConfigParser&operator=(const ConfigParser& src);

    std::vector<ServerTraits> parseConfig();
    void parseServerBlock();
    ServerTraits getServerTraits() const;
    void removeComment(std::string& str);
    void replaceSpaces(std::string& str);
    bool isModuleName(ft::string& str);
    void fillRouteValue(ServerRoute& route, string& name,std::vector<string>& segments);
    void fillServerValue(ServerTraits& server, string& name, std::vector<string>& segments);
};
void setAddress(ft::string& confAdrss, in_addr_t &address, in_port_t& port);