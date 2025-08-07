add_library(snir_compiler_warnings INTERFACE)
add_library(snir::compiler_warnings ALIAS snir_compiler_warnings)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    target_compile_options(snir_compiler_warnings INTERFACE /W4)
else ()
    target_compile_options(snir_compiler_warnings
        INTERFACE
            -Wall
            -Wextra
            -Wpedantic

            -Wcast-align
            -Wconversion
            -Woverloaded-virtual
            -Wreorder
            -Wshadow
            -Wsign-compare
            # -Wsign-conversion
            -Wstrict-aliasing
            -Wswitch-enum
            -Wuninitialized
            -Wunreachable-code
            -Wunused-parameter
            -Wzero-as-null-pointer-constant
            -Wredundant-decls
    )
endif ()
