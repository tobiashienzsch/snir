#pragma once

#include <cstdint>
#include <format>

namespace snir {

enum struct Type : std::uint8_t
{
#define SNIR_TYPE(Id, Name) Id,
#include "snir/ir/Type.def"
#undef SNIR_TYPE
};

[[nodiscard]] auto parseType(std::string_view source) -> Type;

}  // namespace snir

template<>
struct std::formatter<snir::Type, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Type type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_TYPE(Id, Name) std::string_view{#Name},
#include "snir/ir/Type.def"
#undef SNIR_TYPE
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
