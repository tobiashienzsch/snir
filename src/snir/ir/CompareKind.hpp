#pragma once

#include "fmt/format.h"

#include <array>
#include <cstdint>

namespace snir {

enum struct CompareKind : std::uint8_t
{
#define SNIR_COMPARE_KIND(Id, Name) Id,
#include "snir/ir/CompareKind.def"
};

[[nodiscard]] auto parseCompareKind(std::string_view src) -> CompareKind;

}  // namespace snir

template<>
struct fmt::formatter<snir::CompareKind> : formatter<string_view>
{
    auto format(snir::CompareKind kind, format_context& ctx) const -> format_context::iterator
    {
        static constexpr auto names = std::array{
#define SNIR_COMPARE_KIND(Id, Name) string_view{#Name},
#include "snir/ir/CompareKind.def"
        };

        auto str = names.at(static_cast<std::size_t>(kind));
        return formatter<string_view>::format(str, ctx);
    }
};
