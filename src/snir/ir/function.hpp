#pragma once

#include "snir/ir/instruction.hpp"

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

    friend auto operator==(Function const& lhs, Function const& rhs) noexcept -> bool
    {
        return lhs.type == rhs.type and lhs.name == rhs.name and lhs.arguments == rhs.arguments
           and lhs.blocks == rhs.blocks;
    }
};

}  // namespace snir
