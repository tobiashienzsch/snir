#pragma once

#include "snir/ir/Type.hpp"

#include "fmt/format.h"

#include <array>
#include <cstdint>
#include <variant>

namespace snir {

struct Literal
{
    std::variant<bool, std::int64_t, float, double> value;
};

[[nodiscard]] auto parseLiteral(std::string_view src, Type type) -> Literal;

}  // namespace snir

template<>
struct fmt::formatter<snir::Literal> : formatter<string_view>
{
    auto format(snir::Literal literal, format_context& ctx) const -> format_context::iterator
    {
        auto str = std::visit([](auto val) { return fmt::format("{}", val); }, literal.value);
        return formatter<string_view>::format(str, ctx);
    }
};
