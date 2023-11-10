#pragma once

#include <cstdint>

namespace snir::v2 {

enum struct InstKind : std::uint8_t
{
#define SNIR_INSTRUCTION(Id, Name) Id,
#include "snir/ir/v2/inst_kind.def"
#undef SNIR_INSTRUCTION
};

}  // namespace snir::v2
