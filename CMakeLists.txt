cmake_minimum_required(VERSION 3.25)
project(test_client_server_matrix_task)
set(CMAKE_C_STANDARD 11)

add_executable(client src/client.c src/matrix.c)
add_executable(server src/server.c src/matrix.c)

target_include_directories(client PUBLIC include)
target_include_directories(server PUBLIC include)
