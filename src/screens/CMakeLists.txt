add_library(anisette_screens SHARED
        screens/main_menu.cpp
        screens/library.cpp
)
target_include_directories(anisette_screens PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/screens)
target_link_libraries(anisette_screens PUBLIC anisette_core)