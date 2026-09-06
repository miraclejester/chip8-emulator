cmake_minimum_required(VERSION 3.16)
project(chip8 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(chip8 src/main.cpp)

if(TARGET SDL2::SDL2main)
    target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(chip8 PRIVATE SDL2::SDL2)