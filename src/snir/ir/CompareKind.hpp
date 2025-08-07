#pragma once

#include <array>
#include <cstdint>
#include <format>

namespace snir {

enum struct CompareKind : std::uint8_t
{
#define SNIR_COMPARE_KIND(Id, Name) Id,
#include "snir/ir/CompareKind.def"
};

[[nodiscard]] auto parseCompareKind(std::string_view src) -> CompareKind;

}  // namespace snir

template<>
struct std::formatter<snir::CompareKind, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::CompareKind type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_COMPARE_KIND(Id, Name) std::string_view{#Name},
#include "snir/ir/CompareKind.def"
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
