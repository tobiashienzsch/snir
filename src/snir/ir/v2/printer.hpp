#pragma once

#include "snir/ir/v2/module.hpp"
#include "snir/ir/v2/value.hpp"

#include <map>
#include <ostream>

namespace snir::v2 {

struct Printer
{
    explicit Printer(std::ostream& out);

    auto operator()(Module& module) -> void;

private:
    auto printFunction(Module& module, FunctionDefinition const& func) -> void;
    auto printBasicBlock(Module& module, BasicBlock const& block) -> void;
    [[nodiscard]] auto getLocalId(ValueId value) -> int;

    std::reference_wrapper<std::ostream> _out;
    int _nextLocalValueId{0};
    std::map<ValueId, int> _localValueIds{};
};

}  // namespace snir::v2
