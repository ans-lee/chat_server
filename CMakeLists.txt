cmake_minimum_required(VERSION 3.10.2)

project(chat_server C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(chat_server chat_server.c users.c)
add_executable(chat_client chat_client.c)
