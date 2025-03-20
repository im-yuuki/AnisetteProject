add_library(anisette_screens SHARED
        screens/basic_screen_impl.cpp
)
target_include_directories(anisette_screens PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/screens)
target_link_libraries(anisette_screens PRIVATE anisette_log)