#pragma once

#include "snir/ir/Function.hpp"
#include "snir/ir/InstKind.hpp"

namespace snir {

struct RemoveEmptyBlock
{
    static constexpr auto name = std::string_view{"RemoveEmptyBlock"};

    RemoveEmptyBlock() = default;

    auto operator()(Function& func) -> void
    {
        std::erase_if(func.getBasicBlocks(), [](BasicBlock const& block) {
            return block.instructions.empty();
        });
    }
};

}  // namespace snir
