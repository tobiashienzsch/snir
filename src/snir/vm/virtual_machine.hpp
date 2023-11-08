#pragma once

#include "snir/core/variant.hpp"
#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"
#include "snir/ir/value.hpp"

#include <format>
#include <map>
#include <optional>
#include <span>
#include <stdexcept>

namespace snir {

struct VirtualMachine
{
    explicit VirtualMachine(Function const& func, std::span<Value const> arguments);

    [[nodiscard]] auto getReturnValue() const -> std::optional<Value>;

    auto operator()(ReturnInst const& inst) -> void;
    auto operator()(ConstInst const& inst) -> void;
    auto operator()(AddInst const& inst) -> void;
    auto operator()(SubInst const& inst) -> void;
    auto operator()(MulInst const& inst) -> void;
    auto operator()(DivInst const& inst) -> void;
    auto operator()(ModInst const& inst) -> void;
    auto operator()(AndInst const& inst) -> void;
    auto operator()(OrInst const& inst) -> void;
    auto operator()(XorInst const& inst) -> void;
    auto operator()(ShiftLeftInst const& inst) -> void;
    auto operator()(ShiftRightInst const& inst) -> void;

    // auto operator()(FloatAddInst const& inst) -> void;
    // auto operator()(FloatSubInst const& inst) -> void;
    // auto operator()(FloatMulInst const& inst) -> void;
    // auto operator()(FloatDivInst const& inst) -> void;

    template<typename T>
    auto operator()(T const& inst) -> void
    {
        throw std::runtime_error{std::format("unhandled instruction '{}'", typeid(T).name())};
    }

private:
    template<typename T>
    struct ValueAs
    {
        explicit ValueAs(std::map<Register, Value> const& regs) : memory{&regs} {}

        auto operator()(Register reg) -> T { return std::visit(*this, memory->at(reg)); }

        auto operator()(T v) -> T { return static_cast<T>(v); }

        auto operator()(auto) -> T { throw std::runtime_error{"invalid type"}; }

        std::map<Register, Value> const* memory;
    };

    template<typename Op, typename Inst>
    auto binaryIntegerInst(Inst const& inst) -> void
    {
        if (inst.type != Type::Int64) {
            throw std::runtime_error{"unsupported type"};
        }

        auto lhs = std::visit(ValueAs<int>{_register}, inst.lhs);
        auto rhs = std::visit(ValueAs<int>{_register}, inst.rhs);
        _register.emplace(inst.result, Op{}(lhs, rhs));
    }

    std::map<Register, Value> _register;
    std::optional<Value> _return{std::nullopt};
    bool _exit{false};
};

}  // namespace snir
