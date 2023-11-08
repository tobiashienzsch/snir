#pragma once

#include "snir/ir/instruction.hpp"

#include <string>
#include <vector>

namespace snir {

using BasicBlock = std::vector<Instruction>;

struct Function
{
    Type type;
    std::string name;
    std::vector<Type> arguments;
    std::vector<BasicBlock> blocks;

    friend auto operator==(Function const& lhs, Function const& rhs) -> bool = default;
};

}  // namespace snir
