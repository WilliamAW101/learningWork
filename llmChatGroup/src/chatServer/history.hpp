#ifndef HISTORY_H
#define HISTORY_H

#include <fstream>
#include <filesystem>
#include <iostream>
#include <iterator>

class History {
    public:
        History();
        ~History();
        std::string shortTermMemory(); // basically I will have the model summarize the conversation
        void longTermMemory(); // after some time, the LLM will summarize a chunk of conversations
    private:
        int chatCount, chatThreshold;
        std::ofstream memoryWriter;
        std::ifstream memoryReader;
        std::ofstream longTermMemWriter;
        std::string memFile;
};

#endif