# pass CMake-related variables to the source code
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h @ONLY)

include(utils/CMakeLists.txt)
include(core/CMakeLists.txt)
#include(screens/CMakeLists.txt)

# Main executable
if(NOT WIN32)
    add_executable(${PROJECT_NAME} main.cpp)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 main.cpp resource.rc)
else()
    add_executable(${PROJECT_NAME} main.cpp resource.rc)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE anisette_core)
