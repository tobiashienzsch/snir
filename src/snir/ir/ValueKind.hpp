#pragma once

#include <cstdint>

namespace snir {

enum struct ValueKind : std::uint8_t
{
#define SNIR_VALUE_KIND(Id, Name) Id,
#include "snir/ir/ValueKind.def"
};

}  // namespace snir

template<>
struct std::formatter<snir::ValueKind, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::ValueKind kind, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_VALUE_KIND(Id, Name) std::string_view{#Name},
#include "snir/ir/ValueKind.def"
        };

        auto str = names.at(static_cast<std::size_t>(kind));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
