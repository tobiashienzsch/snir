#include "ControlFlowGraph.hpp"

#include "snir/core/exception.hpp"
#include "snir/core/print.hpp"
#include "snir/ir/Branch.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Instruction.hpp"
#include "snir/ir/Operands.hpp"

namespace snir {

auto ControlFlowGraph::operator()(Function const& func, AnalysisManager<Function>& /*analysis*/)
    -> void
{
    _graph.clear();
    _nodeIds.clear();
    _nextNodeId = 0;
    _registry   = func.getValue().registry();

    auto const& blocks = func.getBasicBlocks();
    if (blocks.empty()) {
        return;
    }

    for (auto const& block : blocks) {
        addBlockToGraph(block);
    }

    auto const order = topologicalSort(_graph);
    println("CFG for '{}': ", func.getIdentifier());
    for (auto node : order) {
        print("{} -> ", int(getValueForId(node)));
    }
    println("return");
}

auto ControlFlowGraph::addBlockToGraph(BasicBlock const& block) -> void
{
    auto branchView = _registry->view<Branch>();

    auto const label = block.label;
    auto const node  = getOrCreateNodeId(label);
    _graph.addIfNotContains(node);

    if (block.instructions.empty()) {
        return;
    }

    auto terminal = Instruction{*_registry, block.instructions.back()};
    auto kind     = terminal.getKind();
    if (kind == InstKind::Return) {
        println("return in block {}", int(getValueForId(node)));
    }
    if (kind == InstKind::Branch) {
        auto const [branch] = branchView.get(terminal);
        auto const dest     = getOrCreateNodeId(branch.iftrue);
        println("branch in block {} to {}", int(getValueForId(node)), int(getValueForId(dest)));
        _graph.addIfNotContains(dest);
        _graph.connect(node, dest);
    }
}

auto ControlFlowGraph::getOrCreateNodeId(ValueId value) -> std::uint32_t
{
    if (auto const id = _nodeIds.find(value); id != _nodeIds.end()) {
        return id->second;
    }

    auto const id = _nextNodeId++;
    _nodeIds.emplace(value, id);
    return id;
}

auto ControlFlowGraph::getValueForId(std::uint32_t id) -> ValueId
{
    for (auto [value, node] : _nodeIds) {
        if (node == id) {
            return value;
        }
    }

    raisef<std::runtime_error>("unkown node id '{}'", id);
}

}  // namespace snir
