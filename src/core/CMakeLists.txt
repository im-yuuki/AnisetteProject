add_library(${PROJECT_NAME}_Core SHARED
        loader.cpp
        video.cpp
        audio.cpp
        event.cpp
)
target_link_libraries(${PROJECT_NAME}_Core PRIVATE ${PROJECT_NAME}_Utils)
target_link_libraries(${PROJECT_NAME}_Core
    PUBLIC
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
target_link_libraries(${PROJECT_NAME}_Core
    PRIVATE 
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

target_include_directories(${PROJECT_NAME}_Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${PROJECT_NAME}_Core DESTINATION ${CMAKE_SOURCE_DIR}/out)
