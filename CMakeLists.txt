cmake_minimum_required(VERSION 3.16)
project(SDL2GameProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src ${CMAKE_BINARY_DIR}/bin)

# Dependencies
find_package(fmt CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)