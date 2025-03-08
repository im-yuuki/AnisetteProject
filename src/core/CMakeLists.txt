add_library(core STATIC
    config.cpp
    renderer.cpp
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME}::core ALIAS core)