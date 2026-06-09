cmake_minimum_required(VERSION 3.28)
project(BR_GENERATOR)

set(CMAKE_CXX_STANDARD 20)

add_executable(BR_GENERATOR
        generator.h
        generator.cpp
        topology.cpp
        topology.h
        ../utils.cpp
        ../utils.h
        ../core/inject.cpp
        ../core/inject.h)

target_compile_definitions(BR_GENERATOR
        PRIVATE DATA_DIR="${DATA_DIR}/"
)

target_compile_definitions(BR_GENERATOR
        PRIVATE CUR_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)