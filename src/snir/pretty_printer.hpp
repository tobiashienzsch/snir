#pragma once

#include "snir/print.hpp"

#include <algorithm>
#include <functional>

namespace snir {

struct PrettyPrinter
{
    static constexpr auto name = std::string_view{"PrettyPrinter"};

    explicit PrettyPrinter(std::FILE* out) : _out{out} {}

    auto operator()(snir::Function const& f) -> void
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

    auto operator()(snir::Block const& block) -> void
    {
        std::ranges::for_each(block, std::bind_front(*this));
    }

    auto operator()(snir::Instruction const& inst) -> void { inst.visit(*this); }

    template<typename T>
        requires(T::args == 0)
    auto operator()(T const& inst) -> void
    {
        println(_out, "  ; {}", inst.name);
    }

    template<typename T>
        requires(T::args == 2)
    auto operator()(T const& inst) -> void
    {
        println(
            _out,
            "  {} = {} {} {} {}",
            inst.destination,
            inst.name,
            inst.type,
            inst.lhs,
            inst.rhs
        );
    }

    auto operator()(snir::ConstInst const& constant) -> void
    {
        println(_out, "  {} = {} {}", constant.destination, constant.type, constant.literal);
    }

    auto operator()(snir::TruncInst const& trunc) -> void
    {
        println(
            _out,
            "  {} = {} {} as {}",
            trunc.destination,
            snir::TruncInst::name,
            trunc.operand,
            trunc.type
        );
    }

    auto operator()(snir::ReturnInst const& ret) -> void
    {
        println(_out, "  {} {}", snir::ReturnInst::name, ret.operand);
    }

private:
    std::FILE* _out;
};

}  // namespace snir
