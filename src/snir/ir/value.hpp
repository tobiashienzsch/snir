#pragma once

#include "snir/ir/register.hpp"

#include <cstdint>
#include <format>
#include <optional>

namespace snir {

using Value = std::variant<bool, int, float, double, Register>;

}  // namespace snir

template<>
struct std::formatter<snir::Value, char> : formatter<string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Value op, FormatContext& fc) const
    {
        auto str = visit([](auto v) { return std::format("{}", v); }, op);
        return formatter<string_view, char>::format(str, fc);
    }
};
