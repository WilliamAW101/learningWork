#include "llamaModel.hpp"
#include <fstream>

LlamaModel::LlamaModel(const std::string &modelPath) : modelPath(modelPath) {
    // initialize the LLM
    llama_backend_init();
    llama_numa_init(params.numa);

    std::ifstream f(modelPath);
    if (!f.good()) {
        throw std::invalid_argument("Model file not found at: " + modelPath);
    }
    f.close();

    // initializing the model
    modelParams = llama_model_default_params();
    modelParams.n_gpu_layers = -1;
    model = llama_model_load_from_file(modelPath.c_str(), modelParams);

    if (model == NULL) {
        throw std::invalid_argument("Unable to load the model");
    }

    vocab = llama_model_get_vocab(model);
}

LlamaModel::~LlamaModel() {
    if (model){
        llama_model_free(model);
        model = nullptr;
        vocab = nullptr;
    }
}

std::string LlamaModel::generateText(const std::string &message) {

    // first tokenize the prompt
    std::vector<llama_token> tokenList;
    std::string formatted = "<bos><start_of_turn>user\n" + message + "<end_of_turn>\n<start_of_turn>model\n";

    params.prompt = formatted;
    params.n_predict = 256; // lets just make this a cap for now

    // total length of the sequences including the prompt
    int n_predict = params.n_predict;

    tokenList = common_tokenize(vocab, params.prompt, true);

    const int n_kv_req = tokenList.size() + n_predict;

    // initializing context
    llama_context_params contextParams = common_context_params_to_llama(params);
    contextParams.n_ctx   = n_kv_req;

    auto sparams = llama_sampler_chain_default_params();
    sparams.no_perf = false;

    llama_sampler *sample = llama_sampler_chain_init(sparams);
    llama_sampler_chain_add(sample, llama_sampler_init_top_k(params.sampling.top_k));
    llama_sampler_chain_add(sample, llama_sampler_init_top_p(params.sampling.top_p, params.sampling.min_keep));
    llama_sampler_chain_add(sample, llama_sampler_init_temp (params.sampling.temp));
    llama_sampler_chain_add(sample, llama_sampler_init_dist (params.sampling.seed));

    llama_context * ctx = llama_init_from_model(model, contextParams);

    if (ctx == NULL) {
        throw std::invalid_argument("Failed to create llama context");
    }
    const int n_ctx = llama_n_ctx(ctx);
    // make sure the KV cache is big enough to hold all the prompt and generated tokens
    if (n_kv_req > n_ctx) {
        throw std::invalid_argument("KV cache not big enough");
    }

    // for (auto id : tokenList) {
    //     std::cout << common_token_to_piece(ctx, id).c_str() << std::endl;;
    // }

    // im grabbing this reference from batched.cpp so only will need one batch
    // since I will not be doing batch tokenization
    llama_batch batch = llama_batch_init(tokenList.size(), 0, 1);

    for (size_t i = 0; i < tokenList.size(); i++) {
        common_batch_add(batch, tokenList[i], i, {0}, false);
    }
    

    if (llama_model_has_encoder(model)) {
        if (llama_encode(ctx, batch)) {
            throw std::invalid_argument("Failed to eval");
        }

        llama_token decoder_start_token_id = llama_model_decoder_start_token(model);
        if (decoder_start_token_id == LLAMA_TOKEN_NULL) {
            decoder_start_token_id = llama_vocab_bos(vocab);
        }

        common_batch_clear(batch);
        common_batch_add(batch, decoder_start_token_id, 0, {0}, false);
    }

    // llama_decode will output logits only for the last token of the prompt
    batch.logits[batch.n_tokens - 1] = true;

    if (llama_decode(ctx, batch) != 0) {
        throw std::invalid_argument("llama_decode() failed");
    }

    // track position after prompt
    int n_cur = batch.n_tokens;

    llama_token new_token;
    std::string generated;
    int generatedTokens = 0;

    while (generatedTokens < n_predict) {
        new_token = llama_sampler_sample(sample, ctx, -1);
        
        // check for end of generation tokens
        if (llama_vocab_is_eog(vocab, new_token)) 
            break;
        
        // check for end of turn token specifically
        if (new_token == llama_vocab_eot(vocab)) 
            break;
    
        std::string piece = common_token_to_piece(ctx, new_token);
    
        generated += piece;
        const std::string messageToFind = "<end_of_turn>";
        size_t position = generated.find(messageToFind);
        if (position != std::string::npos) {
            generated.erase(position, generated.length());
            break;
        } 
    
        common_batch_clear(batch);
        common_batch_add(batch, new_token, n_cur++, {0}, true);
    
        if (llama_decode(ctx, batch) != 0)
            throw std::invalid_argument("decode failed");
    
        generatedTokens++;
    }


    llama_batch_free(batch);
    llama_sampler_free(sample);
    llama_free(ctx);

    return generated;

}