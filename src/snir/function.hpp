#pragma once

#include "snir/instruction.hpp"

#include <string>
#include <vector>

namespace snir {

using Block = std::vector<Instruction>;

struct Function
{
    Type type;
    std::string name;
    std::vector<Type> arguments;
    std::vector<Block> blocks;
};

}  // namespace snir
