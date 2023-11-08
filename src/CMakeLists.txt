cmake_minimum_required(VERSION 3.24)
project(snir VERSION ${CMAKE_PROJECT_VERSION})

add_library(snir)
add_library(snir::snir ALIAS snir)
target_include_directories(snir PUBLIC ${PROJECT_SOURCE_DIR})
target_sources(snir
    PRIVATE
        snir/ir/parser.cpp
        snir/ir/pretty_printer.cpp
        snir/vm/interpreter.cpp
        snir/vm/virtual_machine.cpp
)
