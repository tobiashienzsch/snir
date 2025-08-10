add_library(snir_compiler_warnings INTERFACE)
add_library(snir::compiler_warnings ALIAS snir_compiler_warnings)

if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    target_compile_options(snir_compiler_warnings INTERFACE "/W4")
else ()
    target_compile_options(snir_compiler_warnings
        INTERFACE
            "-Wall"
            "-Wextra"
            "-Wpedantic"

            "-Wcast-align"
            "-Wconversion"
            "-Woverloaded-virtual"
            "-Wredundant-decls"
            "-Wreorder"
            "-Wshadow"
            "-Wsign-compare"
            "-Wstrict-aliasing"
            "-Wswitch-enum"
            "-Wuninitialized"
            "-Wunreachable-code"
            "-Wunused-parameter"
            "-Wzero-as-null-pointer-constant"

            "-Wno-switch-enum"
    )

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(snir_compiler_warnings INTERFACE "-Wsign-conversion")

        if(WIN32)
            # Warns for [[no_unique_address]] being ignored for ABI reasons
            target_compile_options(snir_compiler_warnings INTERFACE "-Wno-unknown-attributes")
        endif()
    endif()
endif ()
