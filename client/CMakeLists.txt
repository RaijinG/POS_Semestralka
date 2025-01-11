cmake_minimum_required(VERSION 3.25)
project(POS C)

add_executable(client main.c game_logic.c terminal_gui.c)
target_link_libraries(client pthread)
