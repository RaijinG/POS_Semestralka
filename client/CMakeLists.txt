cmake_minimum_required(VERSION 3.28)
project(POS C)

add_executable(client main.c gui.c game_logic.c)

find_package(SDL2 REQUIRED)
target_include_directories(client PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(client ${SDL2_LIBRARIES})





