include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(lib_anilog SHARED
        utils/logging.cpp
)
target_link_libraries(lib_anilog PUBLIC spdlog::spdlog)

add_library(lib_anidiscord SHARED
        utils/discord_rpc.cpp
)
target_include_directories(lib_anidiscord PRIVATE ${GAME_SDK_INCLUDE})
target_link_libraries(lib_anidiscord PRIVATE ${GAME_SDK})

add_library(lib_anicore SHARED
        core/loader.cpp
        core/video.cpp
        core/audio.cpp
        core/event.cpp
)
target_link_libraries(lib_anicore PRIVATE lib_anilog)
target_link_libraries(lib_anicore PUBLIC SDL3::SDL3)
target_link_libraries(lib_anicore
        PUBLIC
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
)
target_link_libraries(lib_anicore PUBLIC $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf>,SDL3_ttf::SDL3_ttf,SDL3_ttf::SDL3_ttf-static>)
target_link_libraries(lib_anicore PUBLIC SDL3_mixer::SDL3_mixer)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        lib_anicore
        lib_anilog
)

add_library(${PROJECT_NAME}::Core ALIAS lib_anicore)
add_library(${PROJECT_NAME}::Logging ALIAS lib_anilog)
add_library(${PROJECT_NAME}::Discord ALIAS lib_anidiscord)
