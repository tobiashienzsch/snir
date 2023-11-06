#pragma once

#include <cstdint>
#include <format>

namespace snir {

using Literal = std::variant<bool, int, float, double>;

}  // namespace snir

template<>
struct std::formatter<snir::Literal, char> : formatter<string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Literal lit, FormatContext& fc) const
    {
        auto fmt = [](auto l) { return std::format("{}", l); };
        return formatter<string_view, char>::format(visit(fmt, lit), fc);
    }
};
