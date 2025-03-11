add_subdirectory(utils)
add_subdirectory(core)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PROJECT_NAME}_Core
        ${PROJECT_NAME}_Utils
)