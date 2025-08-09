#pragma once

#include "fmt/format.h"

#include <exception>
#include <stdexcept>

namespace snir {

template<typename E, typename... Args>
[[noreturn]] auto raise(Args&&... args)
{
    throw E{std::forward<Args>(args)...};
}

template<typename E, typename... Args>
[[noreturn]] auto raisef(fmt::format_string<Args...> fmt, Args&&... args)
{
    raise<E>(fmt::format(fmt, std::forward<Args>(args)...));
}

}  // namespace snir
