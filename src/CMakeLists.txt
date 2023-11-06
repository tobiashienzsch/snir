cmake_minimum_required(VERSION 3.24)
project(snir VERSION ${CMAKE_PROJECT_VERSION})

add_library(snir)
add_library(snir::snir ALIAS snir)
target_sources(snir PRIVATE snir/core/file.cpp snir/ir/parser.cpp)
target_include_directories(snir PUBLIC ${PROJECT_SOURCE_DIR})
