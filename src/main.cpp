#include "webserv.hpp"

int main(int ac,char** av,char ** envp)
{
    
(void)ac;
(void)envp;
try
{
        ft::string file = av[1] ;
        ConfigParser parser(file);
        std::vector<ServerTraits> conf = parser.parseConfig();

        // Verify server settings
        std::cout << "Server root: " << parser.getServerTraits().root << std::endl;
        std::cout << "Listen address: " << parser.getServerTraits().listen_address << std::endl;
        std::cout << "Listen port: " << parser.getServerTraits().listen_port << std::endl;
        std::cout << "Server name: " << parser.getServerTraits().server_name[0] << std::endl;
        std::cout << "Client max body size: " << parser.getServerTraits().client_max_body_size << std::endl;


        ServerManager serverManager(conf);
        serverManager.run();
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
return(0);
}
   