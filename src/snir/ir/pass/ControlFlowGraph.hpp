#pragma once

#include "snir/graph/Graph.hpp"
#include "snir/ir/AnalysisManager.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Registry.hpp"

#include <algorithm>
#include <map>

namespace snir {

struct ControlFlowGraph
{
    static constexpr auto name = std::string_view{"ControlFlowGraph"};

    struct Result
    {
        std::map<ValueId, std::uint32_t> nodeIds;
        Graph<std::uint32_t> graph;
    };

    ControlFlowGraph() = default;

    auto operator()(Function const& func, AnalysisManager<Function>& analysis) -> void;

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
