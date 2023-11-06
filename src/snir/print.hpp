#pragma once

#include <cstdio>
#include <format>
#include <iostream>

namespace std {

template<typename... Args>
inline auto print(std::FILE* out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    auto str = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(out, "%s", str.c_str());
}

template<typename... Args>
inline auto println(std::FILE* out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    auto str = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(out, "%s\n", str.c_str());
}

template<typename... Args>
inline auto print(std::ostream& out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    out << std::format(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline auto println(std::ostream& out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    out << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

template<typename... Args>
inline auto print(std::format_string<Args...> fmt, Args&&... args) -> void
{
    auto str = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(::stdout, "%s", str.c_str());
}

template<typename... Args>
inline auto println(std::format_string<Args...> fmt, Args&&... args) -> void
{
    auto str = std::format(fmt, std::forward<Args>(args)...);
    std::fprintf(::stdout, "%s\n", str.c_str());
}

}  // namespace std
