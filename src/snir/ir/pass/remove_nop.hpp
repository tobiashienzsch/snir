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
            std::erase_if(block, [](Instruction const& inst) {
                return inst.visit([](auto i) { return std::same_as<decltype(i), NopInst>; });
            });
        }
    }
};

}  // namespace snir
