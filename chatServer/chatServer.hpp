#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <cstring>
#include <vector>

class ChatServer {
    public:
        ChatServer();
        void createClient(sockaddr_in serverAddress, int client);
        void startServer();
        void stopServer();
        bool sClientActive();
    private:
        sockaddr_in serverAddress;
        int serverSocket;
        std::atomic<bool> stopThread;
        std::atomic<int> sharedCounter;
        void handleClient(int clientSocket);

};

#endif