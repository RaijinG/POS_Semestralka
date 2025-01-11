cmake_minimum_required(VERSION 3.25)
project(POS C)

add_executable(server server.c)
target_link_libraries(server pthread)