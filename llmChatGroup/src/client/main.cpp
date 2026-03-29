#include "client.hpp"


int main() {
    // First create a client
    Client client;
    std::thread clientThread(&Client::createClient, &client);
    
    std::string message;
    do{
        std::cout << "Please enter a message (type exit to shutdown): " << std::endl;
        std::getline(std::cin, message);
        client.sendMessage(message);
    } while (message != "exit");

    client.shutdownClient();

    if (clientThread.joinable())
        clientThread.join();

    
    return 0;
}