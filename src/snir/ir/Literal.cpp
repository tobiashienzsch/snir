#include "Literal.hpp"

#include "snir/core/Exception.hpp"
#include "snir/core/Strings.hpp"
#include "snir/ir/Type.hpp"

#include <ctre.hpp>

#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace snir {

auto parseLiteral(std::string_view src, Type type) -> Literal
{
    if (auto const m = ctre::match<R"(([\d]+(|\.[\d]+)))">(src); m) {
        if (type == Type::Int64) {
            return Literal{strings::parse<std::int64_t>(m.get<1>())};
        }
        if (type == Type::Float) {
            return Literal{strings::parse<float>(m.get<1>())};
        }
        if (type == Type::Double) {
            return Literal{strings::parse<double>(m.get<1>())};
        }
    }

    raisef<std::invalid_argument>("failed to parse '{}' as Literal", src);
}

}  // namespace snir
