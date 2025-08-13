project(snir-test VERSION ${CMAKE_PROJECT_VERSION})

add_executable(snir-test-graph)
target_sources(snir-test-graph PRIVATE graph.cpp)
target_link_libraries(snir-test-graph PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_test_graph COMMAND $<TARGET_FILE:snir-test-graph> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-test-interpreter)
target_sources(snir-test-interpreter PRIVATE interpreter.cpp)
target_link_libraries(snir-test-interpreter PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_test_interpreter COMMAND $<TARGET_FILE:snir-test-interpreter> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-test-parser)
target_sources(snir-test-parser PRIVATE parser.cpp)
target_link_libraries(snir-test-parser PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_test_parser COMMAND $<TARGET_FILE:snir-test-parser> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-test-vector)
target_sources(snir-test-vector PRIVATE vector.cpp)
target_link_libraries(snir-test-vector PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_test_vector COMMAND $<TARGET_FILE:snir-test-vector> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-test-v4)
target_sources(snir-test-v4 PRIVATE v4.cpp)
target_link_libraries(snir-test-v4 PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_test_v4 COMMAND $<TARGET_FILE:snir-test-v4> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-test-lang-lexer)
target_sources(snir-test-lang-lexer PRIVATE lang/Lexer.cpp)
target_link_libraries(snir-test-lang-lexer PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_test_lang-lexer COMMAND $<TARGET_FILE:snir-test-lang-lexer> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(snir-test-lang-parser)
target_sources(snir-test-lang-parser PRIVATE lang/Parser.cpp)
target_link_libraries(snir-test-lang-parser PRIVATE snir::snir snir::compiler_warnings)
add_test(NAME snir_test_lang-parser COMMAND $<TARGET_FILE:snir-test-lang-parser> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

file(GLOB SNIR_TEST_FILES "files/*.ll")
foreach(test_file_path IN LISTS SNIR_TEST_FILES)
    get_filename_component(test_file ${test_file_path} NAME)
    add_test(
        NAME
            "snir-opt: ${test_file}"
        COMMAND
            $<TARGET_FILE:snir-opt> -O2 -v -o ${test_file} ${test_file_path}
    )
endforeach()
