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
        void startServer();
    private:
        void handleClient(int clientSocket);
        void broadCastMessage(const std::string &message, int clientSenderSocket);
        void stopServer();

        int serverSocket;
        sockaddr_in serverAddress;
        std::atomic<bool> stopThread;
        std::vector<int> clientSockets;
        std::mutex clientLock;
        std::atomic<int> clientCounter;

};

#endif