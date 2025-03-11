add_library(${PROJECT_NAME}_Utils SHARED
        logging.cpp
)
target_link_libraries(${PROJECT_NAME}_Utils PUBLIC spdlog::spdlog)
target_include_directories(${PROJECT_NAME}_Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${PROJECT_NAME}_Utils DESTINATION ${CMAKE_SOURCE_DIR}/out)
