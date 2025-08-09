#pragma once

#include "snir/core/FlatSet.hpp"
#include "snir/ir/AnalysisManager.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Instruction.hpp"
#include "snir/ir/Operands.hpp"
#include "snir/ir/Result.hpp"
#include "snir/ir/Value.hpp"

#include <ranges>

namespace snir {

struct DeadStoreElimination
{
    static constexpr auto name = std::string_view{"DeadStoreElimination"};

    DeadStoreElimination() = default;

    auto operator()(Function& f, AnalysisManager<Function>& /*analysis*/) -> void
    {
        _used.clear();
        auto* reg    = f.asValue().registry();
        auto& blocks = f.basicBlocks();

        auto nop = Instruction::create(*reg, InstKind::Nop, Type::Void);

        for (auto& block : std::ranges::reverse_view(blocks)) {
            auto rb = std::rbegin(block.instructions);
            auto re = std::rend(block.instructions);
            std::transform(rb, re, rb, [this, reg, nop](auto id) {
                return replaceWithNopIfUnused(Value{*reg, id}, nop);
            });
        }
    }

private:
    auto replaceWithNopIfUnused(Value inst, ValueId nop) -> ValueId
    {
        auto const* result = inst.try_get<Result>();
        if (result != nullptr and not _used.contains(result->id)) {
            return nop;
        }

        auto const* operands = inst.try_get<Operands>();
        if (operands != nullptr) {
            for (auto const op : operands->list) {
                _used.insert(op);
            }
        }

        return inst;
    }

    FlatSet<ValueId> _used;
};

}  // namespace snir
