cmake_minimum_required(VERSION 3.24)
project(snir VERSION ${CMAKE_PROJECT_VERSION})

add_library(snir)
add_library(snir::snir ALIAS snir)
target_include_directories(snir PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(snir PUBLIC ctre::ctre EnTT::EnTT snir::compiler_warnings)
target_sources(snir
    PRIVATE
        snir/ir/CompareKind.cpp
        snir/ir/Identifier.cpp
        snir/ir/InstKind.cpp
        snir/ir/Interpreter.cpp
        snir/ir/Literal.cpp
        snir/ir/Parser.cpp
        snir/ir/PassManager.cpp
        snir/ir/Printer.cpp
        snir/ir/Type.cpp
)
