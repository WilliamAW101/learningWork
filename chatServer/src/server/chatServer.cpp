#include "chatServer.hpp"

ChatServer::ChatServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    stopThread = false;
}

void ChatServer::startServer(){
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    while(!stopThread.load(std::memory_order_relaxed)) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        clientCounter.fetch_add(1);
        if (clientSocket < 0)
            break;
            
        std::thread clientThread(&ChatServer::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void ChatServer::stopServer() { 
    stopThread.store(true, std::memory_order_relaxed);
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
}

void ChatServer::handleClient(int clientSocket) {

    {
        std::unique_lock<std::mutex> lock(clientLock);
        clientSockets.push_back(clientSocket);
    }

    while (1) {
        char buffer[1024] = { 0 };
        if(recv(clientSocket, buffer, sizeof(buffer), 0) > 0){
            std::cout << "Message from client: " << buffer << "\n";
            broadCastMessage(buffer, clientSocket);
        }
        else
            break;
    }

    {
        std::unique_lock<std::mutex> lock(clientLock);
        auto socketindex = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
        if (socketindex != clientSockets.end())
            clientSockets.erase(socketindex);
    }
    close(clientSocket);
    clientCounter.fetch_sub(1);
    if (clientCounter.load() <= 0)
            stopServer();
}

void ChatServer::broadCastMessage(const std::string &message, int clientSenderSocket){
    std::unique_lock<std::mutex> lock(clientLock);
    for(int clientSocket : clientSockets) {
        if (clientSocket != clientSenderSocket)
            send(clientSocket, message.c_str(), message.length(), 0);
    }
}