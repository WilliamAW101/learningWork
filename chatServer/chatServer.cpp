#include "chatServer.hpp"

ChatServer::ChatServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // specifying the address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    stopThread = false;
    sharedCounter = 0;
}

void ChatServer::startServer(){
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    while(!stopThread.load(std::memory_order_relaxed)) {
        int clientSocket= accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0)
            break;

        std::thread clientThread(&ChatServer::handleClient, this, clientSocket);
        clientThread.detach();
    }
    close(serverSocket);
}

void ChatServer::stopServer() { 
    stopThread.store(true, std::memory_order_relaxed);
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
}

void ChatServer::createClient(sockaddr_in serverAddress, int client) {
    sharedCounter.fetch_add(1);
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    connect(clientSocket, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress));
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // sending data
        std::string intStr = std::to_string(client);
        const char* message = "Hello, server! From Client: ";
        std::string fullMessage = std::string(message) + intStr + "\n";
        send(clientSocket, fullMessage.c_str(), fullMessage.length(), 0);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    sharedCounter.fetch_sub(1); 
}

bool ChatServer::sClientActive() {
    if (sharedCounter > 0){
        return true;
    }
    else {
         std::cout << "Clients are all done" << std::endl;
         return false;
    }
}

void ChatServer::handleClient(int clientSocket) {
    while (1) {
        char buffer[1024] = { 0 };
        if(recv(clientSocket, buffer, sizeof(buffer), 0) > 0)
            std::cout << "Message from client: " << buffer << "\n";
        else
            break;
    }
    close(clientSocket);
}