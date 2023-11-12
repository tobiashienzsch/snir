#pragma once

#include "snir/ir/Type.hpp"

#include <cstdint>
#include <format>
#include <variant>

namespace snir::v3 {

struct Literal
{
    std::variant<bool, std::int64_t, float, double> value;
};

[[nodiscard]] auto parseLiteral(std::string_view src, Type type) -> Literal;

}  // namespace snir::v3

template<>
struct std::formatter<snir::v3::Literal, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::v3::Literal literal, FormatContext& fc) const
    {
        auto str = std::visit([](auto val) { return std::format("{}", val); }, literal.value);
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
