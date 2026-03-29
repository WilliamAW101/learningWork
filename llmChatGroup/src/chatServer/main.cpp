#include "llamaModel.hpp"

int main(){
    LlamaModel MIRA("/home/angelo/randomCrap/llmChatGroup/models/gemma.gguf");
    std::string message = MIRA.generateText("Explain to me what you see");
    std::cout << "START OF MESSAGE\n" << std::endl; 
    std::cout << message << std::endl;

    return 0;
}