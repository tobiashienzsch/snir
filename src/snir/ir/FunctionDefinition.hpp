#pragma once

#include "snir/ir/BasicBlock.hpp"

#include <vector>

namespace snir {

struct FunctionDefinition
{
    std::vector<ValueId> args;
    std::vector<BasicBlock> blocks;
};

}  // namespace snir
