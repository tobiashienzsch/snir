#pragma once

#include <cstdint>
#include <format>

namespace snir::v3 {

enum struct Type : std::uint8_t
{
#define SNIR_TYPE(Id, Name) Id,
#include "snir/ir/v3/Type.def"
#undef SNIR_TYPE
};

[[nodiscard]] auto parseType(std::string_view source) -> Type;

}  // namespace snir::v3

template<>
struct std::formatter<snir::v3::Type, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::v3::Type type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_TYPE(Id, Name) std::string_view{#Name},
#include "snir/ir/v3/Type.def"
#undef SNIR_TYPE
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
