cmake_minimum_required(VERSION 3.24)
project(snir VERSION ${CMAKE_PROJECT_VERSION})

add_library(snir)
add_library(snir::snir ALIAS snir)
target_include_directories(snir PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(snir PUBLIC ctre::ctre EnTT::EnTT snir::compiler_warnings)
target_sources(snir
    PRIVATE
        snir/ir/parser.cpp
        snir/ir/pass_manager.cpp
        snir/ir/pretty_printer.cpp
        snir/ir/v3/Interpreter.cpp
        snir/ir/v3/Parser.cpp
        snir/ir/v3/Printer.cpp
        snir/vm/interpreter.cpp
        snir/vm/virtual_machine.cpp
)
