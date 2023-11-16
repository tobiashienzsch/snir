#pragma once

#include "snir/core/StaticVector.hpp"
#include "snir/ir/ValueId.hpp"

namespace snir {

struct Operands
{
    StaticVector<ValueId, 2> list;
};

}  // namespace snir
