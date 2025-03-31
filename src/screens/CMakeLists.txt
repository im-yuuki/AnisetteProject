add_library(anisette_screens SHARED
        screens/splash.cpp
        screens/library.cpp
        screens/menu.cpp
        screens/settings.cpp
        screens/test.cpp
)
target_include_directories(anisette_screens PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/screens)
target_link_libraries(anisette_screens PUBLIC anisette_core)