#pragma once

#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"

#include <algorithm>
#include <set>

namespace snir {

struct DeadStoreElimination
{
    static constexpr auto name = std::string_view{"DeadStoreElimination"};
    DeadStoreElimination()     = default;

    auto operator()(Function& f) -> void
    {
        _used.clear();
        for (auto block = std::rbegin(f.blocks); block != std::rend(f.blocks); ++block) {
            std::transform(
                std::rbegin(*block),
                std::rend(*block),
                std::rbegin(*block),
                [this](auto const& inst) { return replaceWithNopIfUnused(inst); }
            );
        }
    }

private:
    auto replaceWithNopIfUnused(Instruction const& inst) -> Instruction
    {
        auto const destination = inst.getDestinationRegister();
        if (destination and not _used.contains(*destination)) {
            return NopInst{};
        }

        for (auto const reg : inst.getOperandRegisters()) {
            if (reg) {
                _used.insert(*reg);
            }
        }

        return inst;
    }

    std::set<Register> _used;
};

}  // namespace snir
