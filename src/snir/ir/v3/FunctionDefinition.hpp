#pragma once

#include "snir/ir/v3/BasicBlock.hpp"

#include <vector>

namespace snir::v3 {

struct FunctionDefinition
{
    std::vector<ValueId> args;
    std::vector<BasicBlock> blocks;
};

}  // namespace snir::v3
