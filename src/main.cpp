#include "webserv.hpp"

int main(int ac,char** av,char ** envp)
{
    
(void)ac;
(void)envp;
(void)av;
try
{
        ft::string file = "conf.ini";

        ConfigParser parser(file);
        std::vector<ServerTraits> conf = parser.parseConfig();

        // Verify server settings

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
   