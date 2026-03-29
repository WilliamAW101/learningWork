#ifndef SERVER_H
#define SERVER_H

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
#include <string>
#include "llamaModel.hpp"

class Server {
    public:
        Server();
        void startServer();
        ~Server();
    private:
        void handleClient(int clientSocket);
        void broadCastMessage(const std::string &message, int clientSenderSocket);
        void broadCastMIRAMessage(const std::string &message);
        void stopServer();

        int serverSocket;
        sockaddr_in serverAddress;
        std::atomic<bool> stopThread;
        std::vector<int> clientSockets;
        std::mutex clientLock;
        std::atomic<int> clientCounter;
        LlamaModel *model;

};

#endif