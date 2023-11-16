#pragma once

#include "snir/core/LocalIdMap.hpp"
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
        LocalIdMap<ValueId, int> nodeIds;
        Graph<int> graph;
    };

    ControlFlowGraph() = default;

    auto operator()(Function const& func, AnalysisManager<Function>& analysis) -> Result;

private:
    auto addBlockToGraph(BasicBlock const& block) -> void;

    Registry* _registry{nullptr};
    LocalIdMap<ValueId, int> _nodeIds;
    Graph<int> _graph;
};

}  // namespace snir
