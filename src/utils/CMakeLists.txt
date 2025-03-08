add_library(utils STATIC
    logging.cpp
)
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${PROJECT_NAME}::utils ALIAS utils)