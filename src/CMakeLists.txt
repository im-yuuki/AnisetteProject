include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME}_Logging SHARED
        utils/logging.cpp
)
target_link_libraries(${PROJECT_NAME}_Logging PRIVATE spdlog::spdlog)

add_library(${PROJECT_NAME}_Core SHARED
        core/loader.cpp
        core/video.cpp
        core/audio.cpp
        core/event.cpp
)
target_link_libraries(${PROJECT_NAME}_Core PRIVATE ${PROJECT_NAME}_Logging)
target_link_libraries(${PROJECT_NAME}_Core PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME}_Core
        PRIVATE
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PROJECT_NAME}_Core
        ${PROJECT_NAME}_Logging
)