#pragma once

#include <cstdio>
#include <format>
#include <iostream>

namespace snir {

template<typename... Args>
inline auto print(std::FILE* out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    auto str = std::format(fmt, std::forward<Args>(args)...);
    std::fwrite(str.c_str(), sizeof(char), str.size(), out);
}

template<typename... Args>
inline auto println(std::FILE* out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    print(out, fmt, std::forward<Args>(args)...);
    print(out, "\n");
}

template<typename... Args>
inline auto print(std::format_string<Args...> fmt, Args&&... args) -> void
{
    print(::stdout, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline auto println(std::format_string<Args...> fmt, Args&&... args) -> void
{
    print(fmt, std::forward<Args>(args)...);
    print("\n");
}

}  // namespace snir
