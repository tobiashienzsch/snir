#include "InstKind.hpp"

#include "snir/core/Exception.hpp"
#include "snir/core/Strings.hpp"

#include <algorithm>
#include <array>
#include <ranges>
#include <stdexcept>
#include <string_view>

namespace snir {

auto parseInstKind(std::string_view source) -> InstKind
{
    struct InstName
    {
        InstKind kind;
        std::string_view name;
    };

    static constexpr auto names = std::array{
#define SNIR_INST_KIND(Id, Name) InstName{InstKind::Id, std::string_view{#Name}},
#include "snir/ir/InstKind.def"
    };

    auto const found = std::ranges::find(names, strings::trim(source), &InstName::name);
    if (found == std::ranges::cend(names)) {
        raisef<std::invalid_argument>("failed to parse '{}' as InstKind", source);
    }

    return found->kind;
}

}  // namespace snir
