cmake_minimum_required(VERSION 3.25)
project(POS C)

set(CLIENT_BINARY_DIR ${CMAKE_BINARY_DIR}/client)
set(SERVER_BINARY_DIR ${CMAKE_BINARY_DIR}/server)

add_subdirectory(client ${CLIENT_BINARY_DIR})
add_subdirectory(server ${SERVER_BINARY_DIR})

add_custom_target(build_all DEPENDS client server)