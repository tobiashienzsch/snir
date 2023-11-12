#pragma once

#include <string>
#include <string_view>
#include <utility>

namespace snir {

struct Identifier
{
    std::string text;
};

enum struct IdentifierKind
{
    Global,
    Local,
};

[[nodiscard]] auto parseIdentifier(std::string_view src)
    -> std::pair<IdentifierKind, std::string_view>;

}  // namespace snir
