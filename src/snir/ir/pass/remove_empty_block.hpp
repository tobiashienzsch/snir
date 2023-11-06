#pragma once

#include "snir/ir/function.hpp"

namespace snir {

struct RemoveEmptyBlock
{
    static constexpr auto name = std::string_view{"RemoveEmptyBlock"};
    RemoveEmptyBlock()         = default;

    auto operator()(Function& f) -> void { std::erase_if(f.blocks, &Block::empty); }
};

}  // namespace snir
