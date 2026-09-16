cmake_minimum_required(VERSION 3.20)

project(entity_simulator_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(entity_simulator_benchmark
    benchmark.cpp
    ../src/autobots.cpp
    ../src/entity_factorymanager.cpp
    ../src/package.cpp
    ../src/processinghub.cpp
    ../src/service.cpp
    ../src/warehouse.cpp
)

target_include_directories(entity_simulator_benchmark PRIVATE ../include)

if(MSVC)
    target_compile_options(entity_simulator_benchmark PRIVATE /W4)
else()
    target_compile_options(entity_simulator_benchmark PRIVATE -Wall -Wextra -Wpedantic)
endif()