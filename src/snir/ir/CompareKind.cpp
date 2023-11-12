#include "CompareKind.hpp"

#include "snir/core/exception.hpp"

namespace snir::v3 {

auto parseCompareKind(std::string_view src) -> CompareKind
{
#define SNIR_COMPARE(Op, Name)                                                                       \
    if (src == std::string_view{#Name}) {                                                            \
        return CompareKind::Op;                                                                      \
    }
#include "snir/ir/CompareKind.def"
#undef SNIR_COMPARE

    raisef<std::invalid_argument>("failed to parse '{}' as CompareKind", src);
}

}  // namespace snir::v3
