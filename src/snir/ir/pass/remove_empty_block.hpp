#pragma once

#include "snir/ir/function.hpp"

namespace snir {

struct RemoveEmptyBasicBlock
{
    static constexpr auto name = std::string_view{"RemoveEmptyBasicBlock"};

    RemoveEmptyBasicBlock() = default;

    auto operator()(Function& f) -> void { std::erase_if(f.blocks, &BasicBlock::empty); }
};

}  // namespace snir
