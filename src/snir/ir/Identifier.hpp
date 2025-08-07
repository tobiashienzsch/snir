#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace snir {

struct Identifier
{
    std::string text;
};

enum struct IdentifierKind : std::uint8_t
{
    Global,
    Local,
};

[[nodiscard]] auto parseIdentifier(std::string_view src
) -> std::pair<IdentifierKind, std::string_view>;

}  // namespace snir
