#pragma once

#include "snir/ir/v3/BasicBlock.hpp"
#include "snir/ir/v3/Function.hpp"
#include "snir/ir/v3/Module.hpp"

#include <map>
#include <ostream>

namespace snir::v3 {

struct Printer
{
    static constexpr auto name = std::string_view{"Printer"};

    explicit Printer(std::ostream& out);

    auto operator()(Module& module) -> void;
    auto operator()(Function& func) -> void;

private:
    auto printFunction(Function& func) -> void;
    auto printBasicBlock(Function& func, BasicBlock const& block) -> void;
    [[nodiscard]] auto getLocalId(ValueId value) -> int;

    std::reference_wrapper<std::ostream> _out;
    int _nextLocalValueId{0};
    std::map<ValueId, int> _localValueIds{};
};

}  // namespace snir::v3
