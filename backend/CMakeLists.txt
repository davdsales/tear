cmake_minimum_required(VERSION 3.10)
project(SistemaBackend C CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# o SQLite vem como codigo-fonte (sqlite/sqlite3.c), entao nao precisa instalar nada
add_executable(sistema main.cpp sqlite/sqlite3.c)

# httplib usa sockets do Windows
if(WIN32)
    target_link_libraries(sistema ws2_32)
endif()