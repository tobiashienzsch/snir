#pragma once

#include "snir/ir/register.hpp"

#include <cstdint>
#include <format>
#include <optional>

namespace snir {

struct Void
{
    friend auto operator==(Void const& lhs, Void const& rhs) noexcept -> bool = default;
};

using Value = std::variant<Void, bool, int, float, double, Register>;

}  // namespace snir

template<>
struct std::formatter<snir::Value, char> : formatter<string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Value op, FormatContext& fc) const
    {
        auto visitor = [](auto v) {
            if constexpr (std::same_as<decltype(v), snir::Void>) {
                return std::string{"void"};
            } else {
                return std::format("{}", v);
            }
        };
        auto str = std::visit(visitor, op);
        return formatter<string_view, char>::format(str, fc);
    }
};
