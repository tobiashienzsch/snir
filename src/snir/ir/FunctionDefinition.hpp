#pragma once

#include "snir/ir/BasicBlock.hpp"

#include <vector>

namespace snir::v3 {

struct FunctionDefinition
{
    std::vector<ValueId> args;
    std::vector<BasicBlock> blocks;
};

}  // namespace snir::v3
