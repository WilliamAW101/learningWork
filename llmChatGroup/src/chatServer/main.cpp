#include "server.hpp"

int main() {
    std::cout << "Starting Main" << std::endl;
    Server server;
    std::thread serverThread(&Server::startServer, &server);
    if (serverThread.joinable()) {
        serverThread.join();
    }
    std::cout << "Server Shutdown" << std::endl;

    return 0;
}