#pragma once

#include "snir/core/LocalIdMap.hpp"
#include "snir/ir/AnalysisManager.hpp"
#include "snir/ir/BasicBlock.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Module.hpp"
#include "snir/ir/pass/ControlFlowGraph.hpp"

#include <map>
#include <ostream>

namespace snir {

struct Printer
{
    static constexpr auto name = std::string_view{"Printer"};

    explicit Printer(std::ostream& out);

    auto operator()(Module& module) -> void;
    auto operator()(Function& func, AnalysisManager<Function>& analysis) -> void;

private:
    auto printFunction(Function& func) -> void;
    auto printFunctionArgs(Function& func) -> void;
    auto printBasicBlock(Function& func, BasicBlock const& block) -> void;

    std::reference_wrapper<std::ostream> _out;
    ControlFlowGraph::Result const* _cfg{nullptr};
    LocalIdMap<ValueId, int> _localIds;
};

}  // namespace snir
