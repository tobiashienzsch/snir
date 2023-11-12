#pragma once

#include <cstdint>
#include <format>

namespace snir::v3 {

enum struct InstKind : std::uint8_t
{
#define SNIR_INSTRUCTION(Id, Name) Id,
#include "snir/ir/v3/InstKind.def"
#undef SNIR_INSTRUCTION
};

}  // namespace snir::v3

template<>
struct std::formatter<snir::v3::InstKind, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::v3::InstKind kind, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_INSTRUCTION(Id, Name) std::string_view{#Name},
#include "snir/ir/v3/InstKind.def"
#undef SNIR_INSTRUCTION
        };

        auto str = names.at(static_cast<std::size_t>(kind));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
