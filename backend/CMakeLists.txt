cmake_minimum_required(VERSION 3.10)
project(teargurumi_backend)

# configura o padrao c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# inclui a pasta de cabecalhos .h
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# adiciona a biblioteca de sockets para a httplib funcionar no windows
if(WIN32)
    set(EXTRA_LIBS ws2_32)
endif()

# cria o executavel principal
add_executable(sistema main.cpp)

# vincula as bibliotecas no executavel
target_link_libraries(sistema PRIVATE ${EXTRA_LIBS})