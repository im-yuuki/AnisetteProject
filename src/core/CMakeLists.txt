# Game core
add_library(libani_core SHARED
        core/loader.cpp
        core/video.cpp
        core/audio.cpp
        core/event.cpp
)
target_include_directories(libani_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core)
target_link_libraries(libani_core PRIVATE libani_log libani_discord)
# link SDL3 and its extensions
target_link_libraries(libani_core PUBLIC
        $<IF:$<TARGET_EXISTS:SDL3::SDL3-static>,SDL3::SDL3-static,SDL3::SDL3>
)
target_link_libraries(libani_core PUBLIC
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-static>,SDL3_image::SDL3_image-static,SDL3_image::SDL3_image-shared>
)
target_link_libraries(libani_core PUBLIC
        $<IF:$<TARGET_EXISTS:SDL3_ttf::SDL3_ttf-static>,SDL3_ttf::SDL3_ttf-static,SDL3_ttf::SDL3_ttf>
)
target_link_libraries(libani_core PUBLIC
        $<IF:$<TARGET_EXISTS:SDL3_mixer::SDL3_mixer-static>,SDL3_mixer::SDL3_mixer-static,SDL3_mixer::SDL3_mixer>
)

# aliases
add_library(${PROJECT_NAME}::Core ALIAS libani_core)