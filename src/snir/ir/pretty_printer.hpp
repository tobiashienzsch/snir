#pragma once

#include "snir/core/print.hpp"
#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"
#include "snir/ir/module.hpp"

#include <functional>
#include <ostream>

namespace snir {

struct PrettyPrinter
{
    static constexpr auto name = std::string_view{"PrettyPrinter"};

    explicit PrettyPrinter(std::ostream& out);

    auto operator()(Module const& m) -> void;
    auto operator()(Function const& f) -> void;
    auto operator()(Block const& block) -> void;
    auto operator()(Instruction const& inst) -> void;

    auto operator()(NopInst const& nop) -> void;
    auto operator()(ConstInst const& constant) -> void;
    auto operator()(TruncInst const& trunc) -> void;
    auto operator()(IntCmpInst const& ret) -> void;
    auto operator()(ReturnInst const& ret) -> void;

    template<typename Inst>
        requires(Inst::args == 2)
    auto operator()(Inst const& inst) -> void
    {
        println(_out, "  {} = {} {} {} {}", inst.result, inst.name, inst.type, inst.lhs, inst.rhs);
    }

private:
    std::reference_wrapper<std::ostream> _out;
};

}  // namespace snir
