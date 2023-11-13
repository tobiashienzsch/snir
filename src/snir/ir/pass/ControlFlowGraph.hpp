#pragma once

#include "snir/graph/Graph.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Registry.hpp"

#include <algorithm>
#include <map>

namespace snir {

struct ControlFlowGraph
{
    static constexpr auto name = std::string_view{"ControlFlowGraph"};

    ControlFlowGraph() = default;

    auto operator()(Function const& func) -> void;

private:
    auto addBlockToGraph(BasicBlock const& block) -> void;
    [[nodiscard]] auto getOrCreateNodeId(ValueId value) -> std::uint32_t;
    [[nodiscard]] auto getValueForId(std::uint32_t id) -> ValueId;

    std::uint32_t _nextNodeId{0};
    std::map<ValueId, std::uint32_t> _nodeIds;
    Graph<std::uint32_t> _graph;
    Registry* _registry;
};

}  // namespace snir
