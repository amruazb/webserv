#include "Client.hpp"


Client::Client(int fd) : fd_(fd) 
{

}



void Client::sendResponse() 
{
    // Send response to client
    std::string response = "Hello, client!";
    send(fd_, response.c_str(), response.length(), 0);
}
void Client::addPoll(struct pollfd & pfd)
{
    pfd.fd = fd_;
    pfd.events = POLLIN | POLLOUT;
    pfd.revents = 0;
}
int Client::getFd()
{
    return fd_;
}

void Client::close_client() 
{
    // Close client connection
    close(fd_);
}
Client::~Client() 
{
   
}
Client* getClientByFd(std::vector<Client>& clients, int fd) {
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->getFd() == fd) {
            return &(*it);
        }
    }
    return NULL;
}

int Client::readData(string &buff)
{
    char buffer[BUFFER_SIZE] ={0};
    int bytes = recv(fd_, buffer, BUFFER_SIZE, 0);
    if (bytes < 0)
    {
        std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
        throw std::runtime_error("Read Error");
    }
    else if (bytes == 0)
    {
        throw std::runtime_error("Client disconnected");   
        return(0);
    }
    buff.append(buffer, bytes);
    return (1);
}
void removeClientByFd(std::vector<Client> &clients, int fd) {
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->getFd() == fd) {
            clients.erase(it); // Remove the client
            return; // Stop after erasing to prevent iterator invalidation
        }
    }
}

void printClients(std::vector<Client> &clients) {
    for (size_t i = 0; i < clients.size(); ++i) {
    }
}