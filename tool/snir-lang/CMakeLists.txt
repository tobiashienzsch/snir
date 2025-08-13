project(snir-lang VERSION ${CMAKE_PROJECT_VERSION})

add_executable(snir-lang)
target_sources(snir-lang PRIVATE main.cpp)
target_link_libraries(snir-lang PRIVATE snir::snir snir::compiler_warnings)
