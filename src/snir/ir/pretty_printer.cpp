#include "pretty_printer.hpp"

#include <algorithm>
#include <functional>

namespace snir {

PrettyPrinter::PrettyPrinter(std::ostream& out) : _out{out} {}

auto PrettyPrinter::operator()(Module const& m) -> void
{
    for (auto const& func : m.functions) {
        std::invoke(*this, func);
    }
}

auto PrettyPrinter::operator()(Function const& f) -> void
{
    auto const args = [](auto const& a) -> std::string {
        if (a.empty()) {
            return "()";
        }

        auto idx  = 0;
        auto list = std::format("({} %{}", a[0], idx++);
        std::for_each(std::next(a.begin()), a.end(), [&](auto arg) {
            list += std::format(", {} %{}", arg, idx++);
        });
        return list + ")";
    }(f.arguments);

    auto id         = 0;
    auto printBlock = [&id, this](auto const& block) {
        println(_out, "{}:", id++);
        std::invoke(*this, block);
    };

    println(_out, "define {} @{}{} {{", f.type, f.name, args);
    std::ranges::for_each(f.blocks, printBlock);
    println(_out, "}}\n");
}

auto PrettyPrinter::operator()(BasicBlock const& block) -> void
{
    std::ranges::for_each(block, std::bind_front(*this));
}

auto PrettyPrinter::operator()(Instruction const& inst) -> void { inst.visit(*this); }

auto PrettyPrinter::operator()(NopInst const& /*nop*/) -> void
{
    println(_out, "  ; {}", snir::NopInst::name);
}

auto PrettyPrinter::operator()(ConstInst const& inst) -> void
{
    println(_out, "  {} = {} {}", inst.result, inst.type, inst.value);
}

auto PrettyPrinter::operator()(TruncInst const& inst) -> void
{
    println(_out, "  {} = {} {} to {}", inst.result, TruncInst::name, inst.value, inst.type);
}

auto PrettyPrinter::operator()(IntCmpInst const& inst) -> void
{
    println(
        _out,
        "  {} = {} {} {} {}, {}",
        inst.result,
        IntCmpInst::name,
        inst.kind,
        inst.type,
        inst.lhs,
        inst.rhs
    );
}

auto PrettyPrinter::operator()(ReturnInst const& inst) -> void
{
    println(_out, "  {} {} {}", ReturnInst::name, inst.type, inst.value);
}

auto PrettyPrinter::operator()(BranchInst const& inst) -> void
{
    if (not inst.condition.has_value()) {
        println(_out, "  {} label {}", BranchInst::name, inst.iftrue);
    }
}

}  // namespace snir
