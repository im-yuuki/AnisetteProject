cmake_minimum_required(VERSION 3.30)
project(SDL2GameProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        src/main.cpp
#        src/core/renderer.cpp
#        src/core/renderer.h
#        src/screens/splash.cpp
#        src/screens/splash.h
#        src/screens/menu.cpp
#        src/screens/menu.h
#        src/screens/settings.cpp
#        src/screens/settings.h
#        src/utils/config.cpp
#        src/utils/config.h
        src/utils/logging.cpp
        src/utils/logging.h
)

# find_package(SDL2 CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME}
#         PRIVATE
#         $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
#         $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
# )

# find_package(SDL2_image CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME}
#         PRIVATE
#         $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
# )