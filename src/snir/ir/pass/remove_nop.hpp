#pragma once

#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"

namespace snir {

struct RemoveNop
{
    static constexpr auto name = std::string_view{"RemoveNop"};
    RemoveNop()                = default;

    auto operator()(Function& func) -> void
    {
        for (auto& block : func.blocks) {
            std::erase_if(block, [](Instruction const& inst) { return inst.hasType<NopInst>(); });
        }
    }
};

}  // namespace snir
