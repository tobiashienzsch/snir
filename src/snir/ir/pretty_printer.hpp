#pragma once

#include "snir/core/print.hpp"
#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"

#include <functional>
#include <ostream>

namespace snir {

struct PrettyPrinter
{
    static constexpr auto name = std::string_view{"PrettyPrinter"};

    explicit PrettyPrinter(std::ostream& out);

    auto operator()(snir::Function const& f) -> void;
    auto operator()(snir::Block const& block) -> void;
    auto operator()(snir::Instruction const& inst) -> void;

    auto operator()(snir::NopInst const& nop) -> void;
    auto operator()(snir::ConstInst const& constant) -> void;
    auto operator()(snir::TruncInst const& trunc) -> void;
    auto operator()(snir::ReturnInst const& ret) -> void;

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
