cmake_minimum_required(VERSION 3.10)
project(SistemaBackend)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(sistema main.cpp)

# Garante a vinculação da biblioteca do Winsock no Windows
if(WIN32)
    target_link_libraries(sistema PRIVATE ws2_32)
endif()