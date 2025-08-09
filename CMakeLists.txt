cmake_minimum_required(VERSION 3.28...3.29)

find_program(CCACHE ccache)
if (CCACHE)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

project(snir-dev VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(ctre GIT_REPOSITORY "https://github.com/hanickadot/compile-time-regular-expressions" GIT_TAG "v3.10.0")
FetchContent_Declare(entt GIT_REPOSITORY "https://github.com/skypjack/entt" GIT_TAG "v3.15.0")
FetchContent_Declare(fmt GIT_REPOSITORY "https://github.com/fmtlib/fmt" GIT_TAG "11.2.0")
FetchContent_MakeAvailable(ctre entt fmt)

enable_testing()
include(cmake/snir_compiler_warnings.cmake)

add_subdirectory(src)
add_subdirectory(tool/snir-opt)
add_subdirectory(test)
