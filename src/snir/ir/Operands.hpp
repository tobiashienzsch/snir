#pragma once

#include "snir/core/InplaceVector.hpp"
#include "snir/ir/ValueId.hpp"

namespace snir {

struct Operands
{
    InplaceVector<ValueId, 2> list;
};

}  // namespace snir
