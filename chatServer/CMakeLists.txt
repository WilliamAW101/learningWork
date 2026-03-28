cmake_minimum_required(VERSION 3.28.3)
project(ChatApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    src/server/chatServer.cpp
    src/server/main.cpp
)

add_executable(client
    src/client/client.cpp
    src/client/main.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(server Threads::Threads)
target_link_libraries(client Threads::Threads)