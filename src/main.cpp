#include "webserv.hpp"

int main() {
    try {
        Server server;  // Create the server instance
        std::cout << "Server initialized successfully on port 8080.\n";

        while (true) {
            sleep(1); // Keep the server alive
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}