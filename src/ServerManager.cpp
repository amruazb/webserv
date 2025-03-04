#include "ServerManager.hpp"

bool isExit = false;
// 🚀 FIXME:: Ask teammate about storing the info in config
//          ip address and port is needed to initialize the server
//          Discuss how address and ip address (obtained from server.config)are stored
//          needs to change the server constructor accordingly

ServerManager::ServerManager(std::vector<int> ports)
{
    try {
        for (size_t i = 0; i < ports.size(); ++i) {
            servers.emplace_back(ports[i]); // Create servers

            // Add server sockets to poll list
            sockets.push_back((struct pollfd){
                servers.back().getServerFd(),
                POLLIN,
                0
            });

            std::cout << "✅ Server is listening on port " << ports[i] << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        throw;
    }
}
ServerManager::~ServerManager()
{
    for (size_t i = 0; i < sockets.size(); ++i) {
        close(sockets[i].fd);
    }
}
void ServerManager::run()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handle_exit);

    while(!isExit)
    {
        int eventCount = poll(sockets.data(), sockets.size(), -1);
        if (eventCount < 0) 
        {
            std::cerr << "❌ Poll error" << std::endl;
            continue;
        }
        for (size_t i = 0; i < sockets.size(); ++i) 
        {
            struct pollfd &pfd = sockets[i];

            // 🟢 Accept new client connections
            if (pfd.revents & POLLIN && i < servers.size()) 
            {
                int clientFd = accept(pfd.fd, servers[i].getAddress(),servers[i].getAddrlen());
                if (clientFd < 0) {
                    std::cerr << "❌ Accept error" << std::endl;
                    continue;
                }

                // Add new client to poll list
                sockets.push_back((struct pollfd){
                    clientFd, POLLIN | POLLOUT, 0
                });
            }
                // 🔴 Read data from clients
                else if (pfd.revents & POLLIN && i >= servers.size())
                {
                    char buffer[1024] = {0};
                    int bytesRead = recv(pfd.fd, buffer, sizeof(buffer) - 1, 0);
                    
                    if (bytesRead <= 0) {
                        // Client disconnected
                        std::cout << "❌ Client disconnected: FD " << pfd.fd << std::endl;
                        close(pfd.fd);
                        sockets.erase(sockets.begin() + i);
                        clientBuffers.erase(pfd.fd);
                        --i;
                        continue;
                    }
                    clientBuffers[pfd.fd] += std::string(buffer, bytesRead);
                    std::cout << "📩 Received from client FD " << pfd.fd << ": " << buffer << std::endl;
                }
                // Send response to clients
                // 🚀 FIXME: 
                //      1.Ask teammate about handling HTTP request parsing,
                //      2.Need confirmation on expected format of response
                //      3.Implementation of error Handling and custom error Pages


                else if (pfd.revents & POLLOUT && i >= servers.size()) 
                {
                    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
                    send(pfd.fd, response.c_str(), response.length(), 0);

                    std::cout << "📤 Sent response to FD " << pfd.fd << std::endl;

                    // Close connection after response
                    close(pfd.fd);
                    sockets.erase(sockets.begin() + i);
                    clientBuffers.erase(pfd.fd);
                    --i;
                }

            }
    }
}
// Graceful shutdown handler
void handle_exit(int sig)
{
    (void)sig;
    std::cout << "Shutting down the server..." << std::endl;
    isExit = true; // Set a flag to stop the server loop
    // Optionally, clean up any resources here
}