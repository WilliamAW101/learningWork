cmake_minimum_required(VERSION 3.28)
project(MIRAChatGroup)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LLAMA_DIR /home/angelo/randomCrap/llama.cpp)
set(LLAMA_BIN ${LLAMA_DIR}/build/bin)

add_executable(server
    src/chatServer/main.cpp
    src/chatServer/llamaModel.cpp
)

target_include_directories(server PRIVATE
    ${LLAMA_DIR}/include
    ${LLAMA_DIR}/common
    ${LLAMA_DIR}/ggml/include
)

target_link_libraries(server
    ${LLAMA_BIN}/libllama.so
    ${LLAMA_BIN}/libggml.so
    ${LLAMA_BIN}/libggml-base.so
    ${LLAMA_BIN}/libggml-cpu.so
    ${LLAMA_BIN}/libggml-cuda.so
    ${LLAMA_DIR}/build/common/libcommon.a
    ${LLAMA_BIN}/libggml-base.so    # link again after common — resolves circular dep
    ${LLAMA_BIN}/libggml.so         # same
    pthread
    m
    dl
)