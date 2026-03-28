#include "chatServer.hpp"

int main() {
    std::cout << "Starting Main" << std::endl;
    ChatServer server;
    std::thread serverThread(&ChatServer::startServer, &server);
    std::vector<std::thread> clients;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Spawning Clients" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::thread clientThread(&ChatServer::createClient, &server, serverAddress, i);
        clients.push_back(std::move(clientThread));
    }
    std::cout << "Done Spawning Clients" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (server.sClientActive()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    server.stopServer();

    if (serverThread.joinable()) {
        for (int i = 0; i < 5; i++)
            if (clients[i].joinable()){
                clients[i].join();
                std::cout << "Joined Client: " << std::to_string(i) << std::endl;
            }
        serverThread.join();
        std::cout << "Joined Server" << std::endl;
    }
    std::cout << "done" << std::endl;

    return 0;
}