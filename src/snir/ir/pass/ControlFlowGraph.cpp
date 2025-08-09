#include "ControlFlowGraph.hpp"

#include "snir/graph/Graph.hpp"
#include "snir/ir/AnalysisManager.hpp"
#include "snir/ir/BasicBlock.hpp"
#include "snir/ir/Branch.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Instruction.hpp"

#include <print>

namespace snir {

auto ControlFlowGraph::operator()(Function const& func, AnalysisManager<Function>& /*analysis*/)
    -> Result
{
    _graph.clear();
    _nodeIds.clear();
    _registry = func.asValue().registry();

    auto const& blocks = func.basicBlocks();
    if (blocks.empty()) {
        return {};
    }

    std::println("; CFG for '{}': ", func.identifier());
    for (auto const& block : blocks) {
        addBlockToGraph(block);
    }

    std::print("; ");
    auto const order = topologicalSort(_graph);
    for (auto node : order) {
        std::print("{} -> ", int(_nodeIds[node]));
    }
    std::println("return");
    return {
        .nodeIds = _nodeIds,
        .graph   = _graph,
    };
}

auto ControlFlowGraph::addBlockToGraph(BasicBlock const& block) -> void
{
    auto branchView = _registry->view<Branch>();

    auto const label = block.label;
    auto const node  = _nodeIds.add(label);
    _graph.add(node);

    if (block.instructions.empty()) {
        return;
    }

    auto terminal = Instruction{*_registry, block.instructions.back()};
    auto kind     = terminal.kind();
    if (kind == InstKind::Return) {
        std::println("; return in block {}", int(_nodeIds[node]));
    }
    if (kind == InstKind::Branch) {
        auto const [branch] = branchView.get(terminal);
        auto const dest     = _nodeIds.add(branch.iftrue);
        std::println("; branch in block {} to {}", int(_nodeIds[node]), int(_nodeIds[dest]));
        _graph.add(dest);
        _graph.connect(node, dest);
    }
}

}  // namespace snir
