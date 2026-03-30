#include "history.hpp"

History::History(){
    std::cout << "Starting up memory" << std::endl;
    std::string promptFile = "/home/angelo/randomCrap/llmChatGroup/memory/prompt.md";
    memFile = "/home/angelo/randomCrap/llmChatGroup/memory/shortTermMem.md";
    std::string longTermFile = "/home/angelo/randomCrap/llmChatGroup/memory/longTermMem.md";

    memoryWriter.open(memFile, std::ios_base::app);
    longTermMemWriter.open(longTermFile, std::ios_base::app);
    memoryReader.open(promptFile);

    if (!memoryWriter.is_open() || !longTermMemWriter.is_open() || !memoryReader.is_open()) {
        throw std::invalid_argument("Failed to open memory files");
    }

    std::string line;
    while (std::getline(memoryReader, line)) {
        memoryWriter << line << std::endl; // Add newline character back
    }

    memoryReader.close();
    memoryReader.open(memFile);
}

std::string History::shortTermMemory(){
    std::string content(std::istreambuf_iterator<char>(memoryReader), {});
    memoryReader.seekg(0, std::ios::beg);
    return content;
}

History::~History(){
    // wipe short-term memory
    std::filesystem::resize_file(memFile, 0);

    memoryReader.close();
    longTermMemWriter.close();
    memoryWriter.close();
}