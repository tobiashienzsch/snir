cmake_minimum_required(VERSION 3.24)
project(snir-test VERSION ${CMAKE_PROJECT_VERSION})

add_executable(snir-test)
target_sources(snir-test PRIVATE main.cpp)
target_link_libraries(snir-test PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_tests COMMAND $<TARGET_FILE:snir-test> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-ecs-test)
target_sources(snir-ecs-test PRIVATE ecs.cpp)
target_link_libraries(snir-ecs-test PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_ecs_tests COMMAND $<TARGET_FILE:snir-ecs-test> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-v3-test)
target_sources(snir-v3-test PRIVATE v3.cpp)
target_link_libraries(snir-v3-test PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_v3_tests COMMAND $<TARGET_FILE:snir-v3-test> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
