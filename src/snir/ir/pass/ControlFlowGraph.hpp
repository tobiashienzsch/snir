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
        LocalIdMap<ValueId, unsigned> nodeIds;
        Graph<unsigned> graph;
    };

    ControlFlowGraph() = default;

    auto operator()(Function const& func, AnalysisManager<Function>& analysis) -> Result;

private:
    auto addBlockToGraph(BasicBlock const& block) -> void;

    Registry* _registry{nullptr};
    LocalIdMap<ValueId, unsigned> _nodeIds;
    Graph<unsigned> _graph;
};

}  // namespace snir
