#pragma once

#include <cstdint>

namespace snir::v2 {

enum struct ValueKind : std::uint8_t
{
#define SNIR_VALUE(Id, Name) Id,
#include "snir/ir/v2/value_kind.def"
#undef SNIR_VALUE
};

}  // namespace snir::v2
