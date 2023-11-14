#pragma once

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
    [[nodiscard]] auto getLocalId(ValueId value) -> int;

    std::reference_wrapper<std::ostream> _out;
    ControlFlowGraph::Result const* _cfg;
    int _nextLocalValueId{0};
    std::map<ValueId, int> _localValueIds{};
};

}  // namespace snir
