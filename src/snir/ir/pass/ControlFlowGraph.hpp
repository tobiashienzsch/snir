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
        LocalIdMap<ValueId, std::uint32_t> nodeIds;
        Graph<std::uint32_t> graph;
    };

    ControlFlowGraph() = default;

    auto operator()(Function const& func, AnalysisManager<Function>& analysis) -> Result;

private:
    auto addBlockToGraph(BasicBlock const& block) -> void;

    Registry* _registry{nullptr};
    LocalIdMap<ValueId, std::uint32_t> _nodeIds;
    Graph<std::uint32_t> _graph;
};

}  // namespace snir
