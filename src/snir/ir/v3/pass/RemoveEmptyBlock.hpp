#pragma once

#include "snir/ir/v3/Function.hpp"
#include "snir/ir/v3/InstKind.hpp"

namespace snir::v3 {

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

}  // namespace snir::v3
