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
#include <mutex>
#include <algorithm>
#include <memory>

class ChatServer {
    public:
        ChatServer();
        void createClient(sockaddr_in serverAddress, int client);
        void startServer();
        void stopServer();
        bool sClientActive();
        bool checkServerReady();
    private:
        sockaddr_in serverAddress;
        int serverSocket;
        std::atomic<bool> stopThread;
        std::atomic<int> sharedCounter;
        void handleClient(int clientSocket);
        void broadCastMessage(const std::string &message, int clientSenderSocket);
        std::atomic<bool> isServerReady;
        std::vector<int> clientSockets;
        std::mutex clientLock;

};

#endif