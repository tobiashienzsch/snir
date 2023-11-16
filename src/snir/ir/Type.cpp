#include "Type.hpp"

#include "snir/core/Exception.hpp"

namespace snir {

auto parseType(std::string_view source) -> Type
{
#define SNIR_TYPE(Id, Name)                                                                          \
    if (source == std::string_view{#Name}) {                                                         \
        return Type::Id;                                                                             \
    }
#include "snir/ir/Type.def"

    raisef<std::invalid_argument>("failed to parse '{}' as Type", source);
}

}  // namespace snir
