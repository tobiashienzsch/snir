#pragma once

#include <cstdint>
#include <format>

namespace snir {

enum struct Label : std::int32_t
{
};

}  // namespace snir

template<>
struct std::formatter<snir::Label, char> : formatter<string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Label reg, FormatContext& fc) const
    {
        using Int = std::underlying_type_t<snir::Label>;
        auto str  = std::format("%{}", static_cast<Int>(reg));
        return formatter<string_view, char>::format(str, fc);
    }
};
