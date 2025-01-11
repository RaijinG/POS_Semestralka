cmake_minimum_required(VERSION 3.28)
project(POS C)

add_executable(server server.c game_logic.c)
target_link_libraries(server pthread)