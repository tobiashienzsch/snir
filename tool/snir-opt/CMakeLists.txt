cmake_minimum_required(VERSION 3.24)
project(snir-opt VERSION ${CMAKE_PROJECT_VERSION})

add_executable(snir-opt)
target_sources(snir-opt PRIVATE main.cpp)
target_link_libraries(snir-opt PRIVATE snir::snir snir::compiler_warnings)

file(GLOB SNIR_TEST_FILES ${CMAKE_SOURCE_DIR}/test/files/*.ll)
foreach(test_file_path IN LISTS SNIR_TEST_FILES)
    get_filename_component(test_file ${test_file_path} NAME)
    add_test(
        NAME
            "snir-opt: ${test_file}"
        COMMAND
            $<TARGET_FILE:snir-opt> -O2 -v -o ${test_file} ${test_file_path}
    )
endforeach()
