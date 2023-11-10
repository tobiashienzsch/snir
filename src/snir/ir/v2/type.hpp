#pragma once

#include <cstdint>
#include <format>

namespace snir::v2 {

enum struct Type : std::uint8_t
{
#define SNIR_TYPE(Id, Name) Id,
#include "snir/ir/v2/type.def"
#undef SNIR_TYPE
};

}  // namespace snir::v2

template<>
struct std::formatter<snir::v2::Type, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::v2::Type type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_TYPE(Id, Name) std::string_view{#Name},
#include "snir/ir/v2/type.def"
#undef SNIR_TYPE
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
