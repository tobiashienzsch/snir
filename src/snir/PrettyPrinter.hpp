#pragma once

#include "PassManager.hpp"

#include <algorithm>
#include <functional>

namespace snir {

struct PrettyPrinter
{
    static constexpr auto name = std::string_view{"PrettyPrinter"};

    explicit PrettyPrinter(std::FILE* out) : _out{out} {}

    explicit PrettyPrinter(std::ostream& out) : _out{std::ref(out)} {}

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
            writeln("{}:", id++);
            std::invoke(*this, block);
        };

        writeln("define {} @{}{} {{", f.type, f.name, args);
        std::ranges::for_each(f.blocks, printBlock);
        writeln("}}\n");
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
        writeln("  ; {}", inst.name);
    }

    template<typename T>
        requires(T::args == 2)
    auto operator()(T const& inst) -> void
    {
        writeln("  {} = {} {} {} {}", inst.destination, inst.name, inst.type, inst.lhs, inst.rhs);
    }

    auto operator()(snir::ConstInst const& constant) -> void
    {
        writeln("  {} = {} {}", constant.destination, constant.type, constant.literal);
    }

    auto operator()(snir::TruncInst const& trunc) -> void
    {
        writeln(
            "  {} = {} {} as {}",
            trunc.destination,
            snir::TruncInst::name,
            trunc.operand,
            trunc.type
        );
    }

    auto operator()(snir::ReturnInst const& ret) -> void
    {
        writeln("  {} {}", snir::ReturnInst::name, ret.operand);
    }

private:
    using Stream = std::variant<std::FILE*, std::reference_wrapper<std::ostream>>;

    template<typename... Args>
    void write(std::format_string<Args...> fmt, Args&&... args)
    {
        std::visit([&](auto out) { std::print(out, fmt, std::forward<Args>(args)...); }, _out);
    }

    template<typename... Args>
    void writeln(std::format_string<Args...> fmt, Args&&... args)
    {
        std::visit([&](auto out) { std::println(out, fmt, std::forward<Args>(args)...); }, _out);
    }

    Stream _out;
};

}  // namespace snir
