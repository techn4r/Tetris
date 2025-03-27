cmake_minimum_required(VERSION 3.10)

project(Tetris VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

if (TARGET SDL2::SDL2)
    set(SDL2_LIB SDL2::SDL2)
else()
    include_directories(${SDL2_INCLUDE_DIRS})
    set(SDL2_LIB ${SDL2_LIBRARIES})
endif()

add_executable(Tetris tetris.cpp)

target_link_libraries(Tetris PRIVATE ${SDL2_LIB})
