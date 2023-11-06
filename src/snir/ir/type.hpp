#pragma once

#include <concepts>
#include <cstdint>
#include <format>
#include <optional>
#include <variant>

namespace snir {

enum struct Type : std::uint8_t
{
#define SNIR_BUILTIN_TYPE(Identifier, Name) Identifier,
#include "snir/ir/type.def"
#undef SNIR_BUILTIN_TYPE
};

}  // namespace snir

template<>
struct std::formatter<snir::Type, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Type type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_BUILTIN_TYPE(Identifier, Name) std::string_view{#Name},
#include "snir/ir/type.def"
#undef SNIR_BUILTIN_TYPE
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
