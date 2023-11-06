cmake_minimum_required(VERSION 3.24)
project(snir-opt VERSION ${CMAKE_PROJECT_VERSION})

add_executable(snir-opt)
target_sources(snir-opt PRIVATE Main.cpp)
target_link_libraries(snir-opt PRIVATE snir::snir)
add_test(NAME snir_tests COMMAND $<TARGET_FILE:snir-opt> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
