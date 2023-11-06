#pragma once

#include "Function.hpp"
#include "Instruction.hpp"

#include <algorithm>
#include <set>

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

struct RemoveEmptyBlock
{
    static constexpr auto name = std::string_view{"RemoveEmptyBlock"};
    RemoveEmptyBlock()         = default;

    auto operator()(Function& f) -> void { std::erase_if(f.blocks, &Block::empty); }
};

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
