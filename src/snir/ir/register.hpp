#pragma once

#include <cstdint>
#include <format>

namespace snir {

enum struct Register : std::int32_t
{
};

}  // namespace snir

template<>
struct std::formatter<snir::Register, char> : formatter<string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Register reg, FormatContext& fc) const
    {
        using Int = std::underlying_type_t<snir::Register>;
        auto str  = std::format("%{}", static_cast<Int>(reg));
        return formatter<string_view, char>::format(str, fc);
    }
};
