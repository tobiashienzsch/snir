#include "ControlFlowGraph.hpp"

#include "snir/core/Exception.hpp"
#include "snir/core/Print.hpp"
#include "snir/ir/Branch.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Instruction.hpp"
#include "snir/ir/Operands.hpp"

namespace snir {

auto ControlFlowGraph::operator()(Function const& func, AnalysisManager<Function>& /*analysis*/)
    -> Result
{
    _graph.clear();
    _nodeIds.clear();
    _registry = func.getValue().registry();

    auto const& blocks = func.getBasicBlocks();
    if (blocks.empty()) {
        return {};
    }

    println("; CFG for '{}': ", func.getIdentifier());
    for (auto const& block : blocks) {
        addBlockToGraph(block);
    }

    print("; ");
    auto const order = topologicalSort(_graph);
    for (auto node : order) {
        print("{} -> ", int(_nodeIds[node]));
    }
    println("return");
    return {_nodeIds, _graph};
}

auto ControlFlowGraph::addBlockToGraph(BasicBlock const& block) -> void
{
    auto branchView = _registry->view<Branch>();

    auto const label = block.label;
    auto const node  = _nodeIds.add(label);
    _graph.addIfNotContains(node);

    if (block.instructions.empty()) {
        return;
    }

    auto terminal = Instruction{*_registry, block.instructions.back()};
    auto kind     = terminal.getKind();
    if (kind == InstKind::Return) {
        println("; return in block {}", int(_nodeIds[node]));
    }
    if (kind == InstKind::Branch) {
        auto const [branch] = branchView.get(terminal);
        auto const dest     = _nodeIds.add(branch.iftrue);
        println("; branch in block {} to {}", int(_nodeIds[node]), int(_nodeIds[dest]));
        _graph.addIfNotContains(dest);
        _graph.connect(node, dest);
    }
}

}  // namespace snir
