#pragma once

#include "snir/core/exception.hpp"
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

    auto operator()(NopInst const& inst) -> void;
    auto operator()(ReturnInst const& inst) -> void;
    auto operator()(ConstInst const& inst) -> void;
    auto operator()(TruncInst const& inst) -> void;
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
    auto operator()(IntCmpInst const& inst) -> void;
    auto operator()(FloatAddInst const& inst) -> void;
    auto operator()(FloatSubInst const& inst) -> void;
    auto operator()(FloatMulInst const& inst) -> void;
    auto operator()(FloatDivInst const& inst) -> void;

    template<typename Inst>
    auto operator()(Inst const& inst) -> void
    {
        raisef<std::runtime_error>("unhandled instruction '{}'", Inst::name);
    }

private:
    template<typename T>
    struct ValueAs
    {
        explicit ValueAs(std::map<Register, Value> const& regs) : memory{&regs} {}

        auto operator()(Register reg) -> T { return std::visit(*this, memory->at(reg)); }

        auto operator()(T v) -> T { return static_cast<T>(v); }

        auto operator()(auto v) -> T
        {
            raisef<std::runtime_error>(
                "type mismatch for instruction: '{}' vs '{}'",
                typeid(T).name(),
                typeid(decltype(v)).name()
            );
        }

        std::map<Register, Value> const* memory;
    };

    template<typename T>
    struct CastTo
    {
        explicit CastTo(std::map<Register, Value> const& regs) : memory{&regs} {}

        auto operator()(Register reg) -> T { return std::visit(*this, memory->at(reg)); }

        auto operator()(auto v) -> T { return static_cast<T>(v); }

        auto operator()(Void) -> T
        {
            raisef<std::runtime_error>("can't cast void to '{}'", typeid(T).name());
        }

        std::map<Register, Value> const* memory;
    };

    template<typename Inst, typename Op>
    auto binaryIntegerInst(Inst const& inst, Op op) -> void
    {
        if (inst.type == Type::Int64) {
            auto lhs = std::visit(ValueAs<int>{_register}, inst.lhs);
            auto rhs = std::visit(ValueAs<int>{_register}, inst.rhs);
            _register.emplace(inst.result, op(lhs, rhs));
            return;
        }

        raisef<std::runtime_error>("unsupported type for int instruction: '{}'", inst.type);
    }

    template<typename Inst, typename Op>
    auto binaryFloatInst(Inst const& inst, Op op) -> void
    {
        if (inst.type == Type::Float) {
            auto const lhs = std::visit(ValueAs<float>{_register}, inst.lhs);
            auto const rhs = std::visit(ValueAs<float>{_register}, inst.rhs);
            _register.emplace(inst.result, op(lhs, rhs));
            return;
        } else if (inst.type == Type::Double) {
            auto const lhs = std::visit(ValueAs<double>{_register}, inst.lhs);
            auto const rhs = std::visit(ValueAs<double>{_register}, inst.rhs);
            _register.emplace(inst.result, op(lhs, rhs));
            return;
        }

        raisef<std::runtime_error>("unsupported type for float instruction: '{}'", inst.type);
    }

    std::map<Register, Value> _register;
    std::optional<Value> _return{std::nullopt};
    bool _exit{false};
};

}  // namespace snir
