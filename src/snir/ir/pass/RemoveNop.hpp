#pragma once

#include "snir/ir/Function.hpp"
#include "snir/ir/InstKind.hpp"

namespace snir::v3 {

struct RemoveNop
{
    static constexpr auto name = std::string_view{"RemoveNop"};

    RemoveNop() = default;

    auto operator()(Function& func) -> void
    {
        auto instKind = func.getValue().registry()->view<InstKind>();
        for (auto& block : func.getBasicBlocks()) {
            std::erase_if(block.instructions, [&](ValueId id) {
                auto const [kind] = instKind.get(id);
                return kind == InstKind::Nop;
            });
        }
    }
};

}  // namespace snir::v3
