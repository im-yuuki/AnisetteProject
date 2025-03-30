# Game core
add_library(anisette_core SHARED
        core/core.cpp
        core/video.cpp
        core/audio.cpp
        core/frame.cpp
        core/config.cpp
)
target_link_libraries(anisette_core PUBLIC anisette_utils)
# link SDL2 and its extensions
target_link_libraries(anisette_core PUBLIC
        $<IF:$<TARGET_EXISTS:SDL2::SDL2-static>,SDL2::SDL2-static,SDL2::SDL2>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image-static>,SDL2_image::SDL2_image-static,SDL2_image::SDL2_image>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf-static>,SDL2_ttf::SDL2_ttf-static,SDL2_ttf::SDL2_ttf>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer-static>,SDL2_mixer::SDL2_mixer-static,SDL2_mixer::SDL2_mixer>
)
# link rapidjson
target_link_libraries(anisette_core PUBLIC RapidJSON rapidjson)

# aliases
add_library(${PROJECT_NAME}::Core ALIAS anisette_core)