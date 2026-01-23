cmake_minimum_required(VERSION 3.28)
project(BR_USG)

set(CMAKE_CXX_STANDARD 20)

set(DATA_DIR "${CMAKE_SOURCE_DIR}/data" CACHE PATH "Data directory")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

find_package(pybind11 REQUIRED)
find_package(Boost REQUIRED CONFIG)

add_subdirectory(generator)

add_executable(BR_USG
        main.cpp
        core/dsu.cpp
        core/dsu.h
        core/graph.cpp
        core/graph.h
        core/algo.cpp
        core/algo.h
        stat/stat.cpp
        stat/stat.h
        stat/plot.cpp
        stat/plot.h
        experiments.cpp
        experiments.h
)

target_compile_definitions(BR_USG PRIVATE DATA_DIR="${DATA_DIR}/")

target_link_libraries(BR_USG PRIVATE pybind11::embed)
target_link_libraries(BR_USG PRIVATE Boost::boost)