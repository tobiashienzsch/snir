#pragma once

#include "fmt/format.h"

#include <array>
#include <cstdint>

namespace snir {

enum struct ValueKind : std::uint8_t
{
#define SNIR_VALUE_KIND(Id, Name) Id,
#include "snir/ir/ValueKind.def"
};

}  // namespace snir

template<>
struct fmt::formatter<snir::ValueKind> : formatter<string_view>
{
    auto format(snir::ValueKind type, format_context& ctx) const -> format_context::iterator
    {
        static constexpr auto names = std::array{
#define SNIR_VALUE_KIND(Id, Name) string_view{#Name},
#include "snir/ir/ValueKind.def"
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return formatter<string_view>::format(str, ctx);
    }
};
