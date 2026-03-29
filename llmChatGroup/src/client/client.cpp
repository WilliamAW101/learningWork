#include "client.hpp"

Client::Client(){
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr));
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    isClientOn.store(false);
}

void Client::createClient(){
    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    forceStopClient = std::make_shared<std::atomic<bool>>(false);

    std::thread clientListener([this]() {
        char buffer[1024] = { 0 };
        while (1){
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0){
                buffer[bytesRead] = '\0'; // looks like client side is the problem
                std::string bufferString(buffer, bytesRead); // we need to make sure we send the whole message, apparenlty what happens is TCP splits it up and we
                                                             // need to accumilate all the packets untill the end and then send.
                if (forceStopClient->load())
                    break;
                std::cout << buffer << "\n";
            }

        }
    });
    isClientOn.store(true);
    clientListener.detach();
}

void Client::shutdownClient(){
    if (!isClientOn.load()) {
        std::cout << "Client Failed to create" << std::endl;
        return;
    }
    forceStopClient->store(true);
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    isClientOn.store(false);
}

void Client::sendMessage(const std::string &message){
    if (message == "exit")
        return;
    std::string clientPreMessage = "From Client: " + std::to_string(clientSocket) + " : " + message;
    send(clientSocket, clientPreMessage.c_str(), clientPreMessage.length(), 0);
}