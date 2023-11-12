#pragma once

#include "snir/ir/Function.hpp"
#include "snir/ir/Operands.hpp"
#include "snir/ir/Result.hpp"
#include "snir/ir/Value.hpp"

#include "snir/core/print.hpp"

#include <set>

namespace snir::v3 {

struct DeadStoreElimination
{
    static constexpr auto name = std::string_view{"DeadStoreElimination"};

    DeadStoreElimination() = default;

    auto operator()(Function& f) -> void
    {
        _used.clear();
        auto* reg    = f.getValue().registry();
        auto& blocks = f.getBasicBlocks();

        auto nop = reg->create();
        reg->emplace<ValueKind>(nop, ValueKind::Instruction);
        reg->emplace<InstKind>(nop, InstKind::Nop);
        reg->emplace<Type>(nop, Type::Void);

        for (auto block = std::rbegin(blocks); block != std::rend(blocks); ++block) {
            auto rb = std::rbegin(block->instructions);
            auto re = std::rend(block->instructions);
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

    std::set<ValueId> _used;
};

}  // namespace snir::v3
