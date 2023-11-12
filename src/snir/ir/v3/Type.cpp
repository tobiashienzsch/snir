#include "Type.hpp"

#include "snir/core/exception.hpp"

namespace snir::v3 {

auto parseType(std::string_view source) -> Type
{
#define SNIR_TYPE(Id, Name)                                                                          \
    if (source == std::string_view{#Name}) {                                                         \
        return Type::Id;                                                                             \
    }
#include "snir/ir/v3/Type.def"
#undef SNIR_TYPE

    raisef<std::invalid_argument>("failed to parse '{}' as Type", source);
}

}  // namespace snir::v3
