add_library(${PROJECT_NAME}_Logging SHARED
    logging.cpp
)
target_link_libraries(${PROJECT_NAME}_Logging PRIVATE spdlog::spdlog)
target_include_directories(${PROJECT_NAME}_Logging PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${PROJECT_NAME}_Logging DESTINATION ${CMAKE_INSTALL_PREFIX})
