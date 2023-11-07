#pragma once

#include "snir/ir/register.hpp"

#include <cstdint>
#include <format>
#include <optional>

namespace snir {

using Value = std::variant<std::nullopt_t, bool, int, float, double, Register>;

}  // namespace snir

template<>
struct std::formatter<snir::Value, char> : formatter<string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Value op, FormatContext& fc) const
    {
        auto visitor = [](auto v) {
            if constexpr (std::same_as<decltype(v), std::nullopt_t>) {
                return std::format("void");
            } else {
                return std::format("{}", v);
            }
        };
        auto str = visit(visitor, op);
        return formatter<string_view, char>::format(str, fc);
    }
};
