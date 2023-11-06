#pragma once

#include <cstdio>
#include <format>
#include <iostream>

namespace snir {

template<typename... Args>
inline auto print(std::FILE* out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    auto const str     = std::format(fmt, std::forward<Args>(args)...);
    auto const written = std::fwrite(str.c_str(), sizeof(char), str.size(), out);
    if (written != str.size()) {
        throw std::runtime_error{"failed to write to file"};
    }
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
