add_executable(test test.cpp)

target_link_libraries(test PRIVATE GLEW::GLEW)
target_link_libraries(test PRIVATE SDL2::SDL2)