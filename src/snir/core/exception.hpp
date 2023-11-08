#pragma once

#include <exception>
#include <format>
#include <stdexcept>

namespace snir {

template<typename E, typename... Args>
[[noreturn]] auto raise(Args&&... args)
{
    throw E{std::forward<Args>(args)...};
}

template<typename E, typename... Args>
[[noreturn]] auto raisef(std::format_string<Args...> fmt, Args&&... args)
{
    raise<E>(std::format(fmt, std::forward<Args>(args)...));
}

}  // namespace snir
