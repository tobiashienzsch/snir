#pragma once

#include "snir/ir/AnalysisManager.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/InstKind.hpp"

namespace snir {

struct RemoveNop
{
    static constexpr auto name = std::string_view{"RemoveNop"};

    RemoveNop() = default;

    auto operator()(Function& func, AnalysisManager<Function>& /*analysis*/) -> void
    {
        auto instKind = func.asValue().registry()->view<InstKind>();
        for (auto& block : func.basicBlocks()) {
            std::erase_if(block.instructions, [&](ValueId id) {
                auto const [kind] = instKind.get(id);
                return kind == InstKind::Nop;
            });
        }
    }
};

}  // namespace snir
