cmake_minimum_required(VERSION 3.16)
project(Anisette VERSION 1.0.0 LANGUAGES CXX)

# force c++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# support source code in UTF-8
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8")
endif()

# dependencies
find_library(GAME_SDK discord_game_sdk)
find_library(CPP_GAME_SDK discord_game_sdk_cpp)
# spdlog
find_package(spdlog CONFIG REQUIRED)
# SDL3 and its extensions
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
# rapidjson
find_package(RapidJSON CONFIG REQUIRED)

add_subdirectory(src ${CMAKE_BINARY_DIR}/bin)
