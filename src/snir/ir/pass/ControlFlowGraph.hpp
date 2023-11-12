#pragma once

#include "snir/ir/Function.hpp"
#include "snir/ir/InstKind.hpp"

#include "snir/core/print.hpp"

#include <algorithm>

namespace snir {

struct ControlFlowGraph
{
    static constexpr auto name = std::string_view{"ControlFlowGraph"};

    ControlFlowGraph() = default;

    auto operator()(Function const& func) -> void
    {
        auto instKind = func.getValue().registry()->view<InstKind>();
        for (auto const& block : func.getBasicBlocks()) {
            if (block.instructions.empty()) {
                continue;
            }

            auto terminal = block.instructions.back();
            auto [kind]   = instKind.get(terminal);
            if (isTerminal(kind)) {
                println("terminal {}", kind);
            }
        }
    }

private:
    [[nodiscard]] static auto isTerminal(InstKind kind) -> bool
    {
        static constexpr auto const terminals = std::array{
            InstKind::Return,
            InstKind::Branch,
        };

        return std::ranges::find(terminals, kind) != terminals.end();
    }
};

}  // namespace snir
