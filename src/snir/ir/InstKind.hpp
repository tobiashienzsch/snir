#pragma once

#include "fmt/format.h"

#include <array>
#include <cstdint>

namespace snir {

enum struct InstKind : std::uint8_t
{
#define SNIR_INST_KIND(Id, Name) Id,
#include "snir/ir/InstKind.def"
};

[[nodiscard]] auto parseInstKind(std::string_view source) -> InstKind;

}  // namespace snir

template<>
struct fmt::formatter<snir::InstKind> : formatter<string_view>
{
    template<typename FormatContext>
    auto format(snir::InstKind kind, FormatContext& ctx) const
    {
        static constexpr auto names = std::array{
#define SNIR_INST_KIND(Id, Name) string_view{#Name},
#include "snir/ir/InstKind.def"
        };

        auto str = names.at(static_cast<std::size_t>(kind));
        return formatter<string_view>::format(str, ctx);
    }
};
