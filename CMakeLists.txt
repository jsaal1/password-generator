cmake_minimum_required(VERSION 3.25)
project(passgen VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(passgen
    src/main.cpp
    src/parser.cpp
    src/generator.cpp
)

target_include_directories(passgen PRIVATE include)

target_compile_options(passgen PRIVATE -Wall -Wextra -Wpedantic)
