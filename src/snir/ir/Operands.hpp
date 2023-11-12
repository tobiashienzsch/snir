#pragma once

#include "snir/ir/ValueId.hpp"

#include "snir/core/static_vector.hpp"

namespace snir {

struct Operands
{
    StaticVector<ValueId, 2> list;
};

}  // namespace snir
