#pragma once

#include <concepts>
#include <cstdint>
#include <format>
#include <optional>
#include <variant>

namespace snir {

enum struct Type : std::uint8_t
{
    Void,
    Bool,
    Int64,
    Float,
    Double,
    Block,
    Event,
};

enum struct Register : std::int32_t
{
};

using Literal = std::variant<bool, int, float, double>;
using Operand = std::variant<Register, bool, int, float, double>;

}  // namespace snir

template<>
struct std::formatter<snir::Type, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Type type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
            std::string_view{"void"},
            std::string_view{"i1"},
            std::string_view{"i64"},
            std::string_view{"float"},
            std::string_view{"double"},
            std::string_view{"block"},
            std::string_view{"event"},
        };

        auto str = names[static_cast<std::size_t>(type)];
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};

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
