#pragma once

#include <cstdint>
#include <format>

namespace snir {

enum struct InstKind : std::uint8_t
{
#define SNIR_INSTRUCTION(Id, Name) Id,
#include "snir/ir/InstKind.def"
#undef SNIR_INSTRUCTION
};

[[nodiscard]] auto parseInstKind(std::string_view source) -> InstKind;

}  // namespace snir

template<>
struct std::formatter<snir::InstKind, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::InstKind kind, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_INSTRUCTION(Id, Name) std::string_view{#Name},
#include "snir/ir/InstKind.def"
#undef SNIR_INSTRUCTION
        };

        auto str = names.at(static_cast<std::size_t>(kind));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
