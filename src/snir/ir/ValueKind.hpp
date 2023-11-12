#pragma once

#include <cstdint>

namespace snir::v3 {

enum struct ValueKind : std::uint8_t
{
#define SNIR_VALUE(Id, Name) Id,
#include "snir/ir/ValueKind.def"
#undef SNIR_VALUE
};

}  // namespace snir::v3

template<>
struct std::formatter<snir::v3::ValueKind, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::v3::ValueKind kind, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_VALUE(Id, Name) std::string_view{#Name},
#include "snir/ir/ValueKind.def"
#undef SNIR_VALUE
        };

        auto str = names.at(static_cast<std::size_t>(kind));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
