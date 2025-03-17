configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/version.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# Logging library
add_library(lib_anilog SHARED
        utils/logging/logging.cpp
)
target_include_directories(lib_anilog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils/logging)
target_link_libraries(lib_anilog PUBLIC spdlog::spdlog)

# Discord RPC connection
add_library(lib_anidiscord SHARED
        utils/discord/discord.cpp
)
target_include_directories(lib_anidiscord PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/utils/discord)
target_link_libraries(lib_anidiscord PUBLIC discord-rpc)
target_link_libraries(lib_anidiscord PRIVATE lib_anilog)

# Game core
add_library(lib_anicore SHARED
        core/loader.cpp
        core/video.cpp
        core/audio.cpp
        core/event.cpp
)
target_include_directories(lib_anicore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core)
target_link_libraries(lib_anicore PRIVATE lib_anilog lib_anidiscord)
target_link_libraries(lib_anicore PUBLIC SDL3::SDL3)
target_link_libraries(lib_anicore
        PUBLIC
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
)
target_link_libraries(lib_anicore PUBLIC $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,SDL3_ttf::SDL3_ttf,SDL3_ttf::SDL3_ttf-static>)
target_link_libraries(lib_anicore PUBLIC SDL3_mixer::SDL3_mixer-static)


# Main executable
if(NOT WIN32)
    add_executable(${PROJECT_NAME} main.cpp)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 main.cpp resource.rc)
else()
    add_executable(${PROJECT_NAME} main.cpp resource.rc)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE lib_anicore lib_anilog)

add_library(${PROJECT_NAME}::Core ALIAS lib_anicore)
add_library(${PROJECT_NAME}::Logging ALIAS lib_anilog)
add_library(${PROJECT_NAME}::Discord ALIAS lib_anidiscord)

