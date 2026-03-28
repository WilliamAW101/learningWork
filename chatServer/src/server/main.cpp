#include "chatServer.hpp"

int main() {
    std::cout << "Starting Main" << std::endl;
    ChatServer server;
    std::thread serverThread(&ChatServer::startServer, &server);
    if (serverThread.joinable()) {
        serverThread.join();
    }
    std::cout << "Server Shutdown" << std::endl;

    return 0;
}