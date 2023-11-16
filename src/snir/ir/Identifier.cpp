#include "Identifier.hpp"

#include "snir/core/Exception.hpp"

#include <ctre.hpp>

namespace snir {

auto parseIdentifier(std::string_view src) -> std::pair<IdentifierKind, std::string_view>
{
    if (auto const m = ctre::match<R"(([%@][a-zA-Z0-9$._]+))">(src); m) {
        auto str = m.get<1>().view();
        if (str.starts_with("%")) {
            return {IdentifierKind::Local, str.substr(1)};
        }
        return {IdentifierKind::Global, str.substr(1)};
    }

    raisef<std::invalid_argument>("failed to parse '{}' as Identifier", src);
}

}  // namespace snir
