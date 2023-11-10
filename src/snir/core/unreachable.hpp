#pragma once

namespace snir {

[[noreturn]] inline auto unreachable() -> void
{
#if defined(__GNUC__)
    __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(false);
#endif
}

}  // namespace snir
