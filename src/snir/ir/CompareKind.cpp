#include "CompareKind.hpp"

#include "snir/core/exception.hpp"

namespace snir {

auto parseCompareKind(std::string_view src) -> CompareKind
{
#define SNIR_COMPARE_KIND(Op, Name)                                                                  \
    if (src == std::string_view{#Name}) {                                                            \
        return CompareKind::Op;                                                                      \
    }
#include "snir/ir/CompareKind.def"

    raisef<std::invalid_argument>("failed to parse '{}' as CompareKind", src);
}

}  // namespace snir
