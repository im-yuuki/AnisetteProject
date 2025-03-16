cmake_minimum_required(VERSION 3.16)
project(Anisette VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8")
endif()

# Dependencies
add_subdirectory(externals/SDL_mixer EXCLUDE_FROM_ALL) # SDL3_mixer is not currently available in vcpkg, so I have to build it myself
# spdlog
find_package(spdlog CONFIG REQUIRED)
# SDL3 and its extensions
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
# Discord Game SDK
find_library(GAME_SDK discord_game_sdk)
find_path(GAME_SDK_INCLUDE discord-game-sdk/discord.h)

configure_file(${CMAKE_SOURCE_DIR}/version.h.in ${CMAKE_SOURCE_DIR}/src/include/version.h @ONLY)
add_subdirectory(src ${CMAKE_BINARY_DIR}/bin)
