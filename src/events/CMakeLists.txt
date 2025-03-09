add_library(${PROJECT_NAME}_Events SHARED
    route.cpp
)
target_include_directories(${PROJECT_NAME}_Events PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${PROJECT_NAME}_Events DESTINATION ${CMAKE_SOURCE_DIR}/out)