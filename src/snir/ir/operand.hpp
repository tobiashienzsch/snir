#pragma once

#include "snir/ir/register.hpp"

#include <cstdint>
#include <format>

namespace snir {

using Operand = std::variant<Register, bool, int, float, double>;

}  // namespace snir

template<>
struct std::formatter<snir::Operand, char> : formatter<string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Operand op, FormatContext& fc) const
    {
        auto str = visit([](auto v) { return std::format("{}", v); }, op);
        return formatter<string_view, char>::format(str, fc);
    }
};
