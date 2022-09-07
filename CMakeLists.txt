cmake_minimum_required(VERSION 3.10)

project(Game VERSION 0.1.0)
add_executable(Game src/main.cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(deps/glfw)
add_subdirectory(deps/glad)
add_subdirectory(deps/glm)

target_link_libraries(Game glfw)
target_link_libraries(Game glad)
target_link_libraries(Game glm)

configure_file(src/GameConfig.h.in src/GameConfig.h)

target_include_directories(Game PUBLIC "${PROJECT_BINARY_DIR}")

