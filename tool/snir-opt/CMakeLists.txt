cmake_minimum_required(VERSION 3.24)
project(snir-opt VERSION ${CMAKE_PROJECT_VERSION})

add_executable(snir-opt)
target_sources(snir-opt PRIVATE main.cpp)
target_link_libraries(snir-opt PRIVATE snir::snir snir::compiler_warnings)
