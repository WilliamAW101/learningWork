#ifndef LLAMA_MODEL_H
#define LLAMA_MODEL_H

#include <string>
#include "llama.h"
#include "common.h"
#include <iostream>

class LlamaModel {
    public:
        LlamaModel(const std::string& modelPath);
        void startModel();
        ~LlamaModel();
        std::string generateText(const std::string &message);
    private:
        std::string modelPath;
        llama_model* model;
        common_params params;
        llama_model_params modelParams;
        const llama_vocab *vocab;
        int n_parallel, n_predict; 
};

#endif