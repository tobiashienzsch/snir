#pragma once

#include "snir/ir/ValueId.hpp"

#include <vector>

namespace snir::v3 {

struct BasicBlock
{
    ValueId label;
    std::vector<ValueId> instructions;
};

}  // namespace snir::v3
