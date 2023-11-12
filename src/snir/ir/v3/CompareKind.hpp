#pragma once

#include <cstdint>
#include <format>

namespace snir::v3 {

enum struct CompareKind
{
#define SNIR_COMPARE(Id, Name) Id,
#include "snir/ir/v3/CompareKind.def"
#undef SNIR_COMPARE
};

}  // namespace snir::v3

template<>
struct std::formatter<snir::v3::CompareKind, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::v3::CompareKind type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_COMPARE(Id, Name) std::string_view{#Name},
#include "snir/ir/v3/CompareKind.def"
#undef SNIR_COMPARE
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
