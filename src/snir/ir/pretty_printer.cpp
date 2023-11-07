#include "pretty_printer.hpp"

#include <algorithm>
#include <functional>

namespace snir {

PrettyPrinter::PrettyPrinter(std::ostream& out) : _out{out} {}

auto PrettyPrinter::operator()(snir::Function const& f) -> void
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

auto PrettyPrinter::operator()(snir::Block const& block) -> void
{
    std::ranges::for_each(block, std::bind_front(*this));
}

auto PrettyPrinter::operator()(snir::Instruction const& inst) -> void { inst.visit(*this); }

auto PrettyPrinter::operator()(snir::NopInst const& nop) -> void
{
    println(_out, "  ; {}", nop.name);
}

auto PrettyPrinter::operator()(snir::ConstInst const& constant) -> void
{
    println(_out, "  {} = {} {}", constant.result, constant.type, constant.value);
}

auto PrettyPrinter::operator()(snir::TruncInst const& trunc) -> void
{
    println(_out, "  {} = {} {} as {}", trunc.result, snir::TruncInst::name, trunc.value, trunc.type);
}

auto PrettyPrinter::operator()(snir::ReturnInst const& ret) -> void
{
    println(_out, "  {} {}", snir::ReturnInst::name, ret.value);
}

}  // namespace snir
