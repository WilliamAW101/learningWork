#include "client.hpp"


int main() {
    std::cout << "Please enter your name" << std::endl;
    std::string name;
    std::getline(std::cin, name);
    // First create a client
    Client client(name);
    std::thread clientThread(&Client::createClient, &client);
    
    std::string message;
    std::cout << "Please enter a message (type exit to shutdown): " << std::endl;
    do{
        std::getline(std::cin, message);
        client.sendMessage(message);
    } while (message != "exit");

    client.shutdownClient();

    if (clientThread.joinable())
        clientThread.join();

    
    return 0;
}