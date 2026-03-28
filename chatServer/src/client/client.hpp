#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <memory>


class Client {
    public:
        Client();
        void createClient();
        void shutdownClient();
        void sendMessage(const std::string &message);
    private:
        std::atomic<bool> isClientOn;
        int clientSocket;
        sockaddr_in serverAddress; // this will probs be localhost
        std::shared_ptr<std::atomic<bool>> forceStopClient;
};

#endif

