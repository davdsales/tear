cmake_minimum_required(VERSION 3.10)
project(SistemaBackend C CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# SQLite compilado junto com o projeto
add_executable(sistema main.cpp sqlite/sqlite3.c)

# Configuração para Windows
if(WIN32)
    target_compile_definitions(sistema PRIVATE
        _WIN32_WINNT=0x0A00
        WINVER=0x0A00
    )

    target_link_libraries(sistema ws2_32)
endif()