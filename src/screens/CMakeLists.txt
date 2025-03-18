add_library(libani_screens SHARED
        screens/basic_screen_impl.cpp
)
target_include_directories(libani_screens PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/screens)
target_link_libraries(libani_screens PRIVATE libani_log)
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