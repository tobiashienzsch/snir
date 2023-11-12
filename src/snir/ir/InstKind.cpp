#include "InstKind.hpp"

#include "snir/core/exception.hpp"
#include "snir/core/strings.hpp"

#include <algorithm>

namespace snir::v3 {

auto parseInstKind(std::string_view source) -> InstKind
{
    struct InstName
    {
        InstKind kind;
        std::string_view name;
    };

    static constexpr auto names = std::array{
#define SNIR_INSTRUCTION(Id, Name) InstName{InstKind::Id, std::string_view{#Name}},
#include "snir/ir/InstKind.def"
#undef SNIR_INSTRUCTION
    };

    auto const* found = std::ranges::find(names, strings::trim(source), &InstName::name);
    if (found == std::ranges::end(names)) {
        raisef<std::invalid_argument>("failed to parse '{}' as InstKind", source);
    }

    return found->kind;
}

}  // namespace snir::v3
