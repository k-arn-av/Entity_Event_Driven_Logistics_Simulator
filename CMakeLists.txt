cmake_minimum_required(VERSION 3.20)

# Only apply /mucrt when compiling with MSVC; MinGW/GCC does not recognize it.
if(MSVC)
    add_compile_options(/mucrt)
    add_link_options(/mucrt)
endif()

# Project Name and Language
project(entity_simulator LANGUAGES CXX)

# Enforce C++23 Standard (Required for <print>)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all source files from the src directory
set(SOURCES
    src/main.cpp
    src/autobots.cpp
    src/entity_factorymanager.cpp
    src/package.cpp
    src/processinghub.cpp
    src/service.cpp
    src/warehouse.cpp
)

# Define Executable Target
add_executable(entity_simulator ${SOURCES})

# Tell CMake where to find header files (.hpp) in include/
target_include_directories(entity_simulator PRIVATE include)

# Optional Compiler Warnings
if(MSVC)
    target_compile_options(entity_simulator PRIVATE /W4)
else()
    target_compile_options(entity_simulator PRIVATE -Wall -Wextra -Wpedantic)
endif()