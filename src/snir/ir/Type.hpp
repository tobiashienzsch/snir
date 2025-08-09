#pragma once

#include "fmt/format.h"

#include <array>
#include <cstdint>
#include <string_view>

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
struct fmt::formatter<snir::Type> : formatter<string_view>
{
    template<typename FormatContext>
    auto format(snir::Type type, FormatContext& ctx) const
    {
        static constexpr auto names = std::array{
#define SNIR_TYPE(Id, Name) string_view{#Name},
#include "snir/ir/Type.def"
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return formatter<string_view>::format(str, ctx);
    }
};
