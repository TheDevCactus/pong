cmake_minimum_required(VERSION 3.10)

project(Game VERSION 0.1.0)
add_executable(Game src/main.cpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(src/GameConfig.h.in src/GameConfig.h)

target_include_directories(Game PUBLIC "${PROJECT_BINARY_DIR}")

