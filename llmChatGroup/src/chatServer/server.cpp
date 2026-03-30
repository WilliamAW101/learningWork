#include "server.hpp"

Server::Server() {

    // we need to set up the llama model
    model = new LlamaModel("/home/angelo/randomCrap/llmChatGroup/models/gemma.gguf");

    // set up the memory
    history = new History();

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    stopThread = false;
}

void Server::startServer(){
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    while(!stopThread.load(std::memory_order_relaxed)) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        clientCounter.fetch_add(1);
        if (clientSocket < 0)
            break;
            
        std::thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void Server::stopServer() { 
    stopThread.store(true, std::memory_order_relaxed);
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
}

void Server::handleClient(int clientSocket) {
    {
        std::unique_lock<std::mutex> lock(clientLock);
        clientSockets.push_back(clientSocket);
    }

    while (1) {
        char buffer[1024] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string bufferString(buffer, bytesRead); // clearing the buffer string, hopefully this fixes the leaking characters
            std::cout << "Message from client: " << bufferString << "\n";

            broadCastMessage(bufferString, clientSocket); // lets both send to MIRA and all the clients, gives everyone context as to why MIRA responded

            if (bufferString.find("Hey MIRA:") != std::string::npos) {
                char delimiter = ':';
                size_t pos = bufferString.find(delimiter);
                bufferString.erase(0, pos + 1);
                // If I dont run this on seperate thread, text generation can block the loop
                std::thread([this, bufferString]() {
                    std::string response = model->generateText(history->shortTermMemory() + "\n" + bufferString);
                    broadCastMIRAMessage("MIRA: " + response);
                }).detach();
            } 
        } else {
            break;
        }
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
void Server::broadCastMessage(const std::string &message, int clientSenderSocket){
    std::unique_lock<std::mutex> lock(clientLock);
    for(int clientSocket : clientSockets) {
        if (clientSocket != clientSenderSocket)
            send(clientSocket, message.c_str(), message.length(), 0);
    }
}

void Server::broadCastMIRAMessage(const std::string &message) {
    std::unique_lock<std::mutex> lock(clientLock);
    for(int clientSocket : clientSockets) {
        send(clientSocket, message.c_str(), message.length(), 0);
    } 
}

Server::~Server(){
    delete model;
    delete history;
}