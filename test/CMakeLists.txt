cmake_minimum_required(VERSION 3.24)
project(snir-test VERSION ${CMAKE_PROJECT_VERSION})

add_executable(snir-test)
target_sources(snir-test PRIVATE main.cpp)
target_link_libraries(snir-test PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_tests COMMAND $<TARGET_FILE:snir-test> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
