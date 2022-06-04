cmake_minimum_required(VERSION 3.21)
project(proj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(proj main.cpp)
target_link_libraries(
    proj
    sfml-graphics
    sfml-window
    sfml-system
)