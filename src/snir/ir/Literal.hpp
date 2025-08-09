#pragma once

#include "snir/ir/Type.hpp"

#include "fmt/format.h"
#include "fmt/std.h"

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
struct fmt::formatter<snir::Literal> : formatter<std::variant<bool, std::int64_t, float, double>>
{
    template<typename FormatContext>
    auto format(snir::Literal literal, FormatContext& ctx) const
    {
        return formatter<std::variant<bool, std::int64_t, float, double>>::format(literal.value, ctx);
    }
};
