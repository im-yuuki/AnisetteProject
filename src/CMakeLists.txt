add_subdirectory(utils)
add_subdirectory(core)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} LINK_PRIVATE ${PROJECT_NAME}_Logging)
target_link_libraries(${PROJECT_NAME} LINK_PRIVATE ${PROJECT_NAME}_Core)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})