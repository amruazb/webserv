#include "webserv.hpp"

int main() {
    
        std::vector<int> ports;
        ports.push_back(8080);
        ports.push_back(9090);

        ServerManager serverManager(ports);
        serverManager.run();

        return(0);
        }
   