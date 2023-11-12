#pragma once

#include "snir/ir/ValueId.hpp"

#include <vector>

namespace snir {

struct BasicBlock
{
    ValueId label;
    std::vector<ValueId> instructions;
};

}  // namespace snir
