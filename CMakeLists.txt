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
add_subdirectory(externals/discord-rpc EXCLUDE_FROM_ALL) # discord rpc
add_subdirectory(externals/SDL_mixer EXCLUDE_FROM_ALL) # SDL3_mixer is not currently available in vcpkg, so I have to build it myself
# spdlog
find_package(spdlog CONFIG REQUIRED)
# SDL3 and its extensions
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
# asio
find_package(asio CONFIG REQUIRED)

add_subdirectory(src ${CMAKE_BINARY_DIR}/bin)
