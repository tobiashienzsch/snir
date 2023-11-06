#pragma once

#include <format>
#include <iostream>

namespace snir {

template<typename... Args>
inline auto print(std::ostream& out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    out << std::format(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline auto println(std::ostream& out, std::format_string<Args...> fmt, Args&&... args) -> void
{
    print(out, fmt, std::forward<Args>(args)...);
    print(out, "\n");
}

template<typename... Args>
inline auto print(std::format_string<Args...> fmt, Args&&... args) -> void
{
    print(std::cout, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline auto println(std::format_string<Args...> fmt, Args&&... args) -> void
{
    print(fmt, std::forward<Args>(args)...);
    print("\n");
}

}  // namespace snir
