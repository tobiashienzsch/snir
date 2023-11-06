#pragma once

#include "snir/ir/instructions.hpp"
#include "snir/ir/register.hpp"
#include "snir/ir/type.hpp"
#include "snir/ir/value.hpp"

#include <array>

namespace snir {

struct Instruction
{
    template<typename Inst>
        requires(not std::same_as<Inst, Instruction>)
    // NOLINTNEXTLINE(hicpp-explicit-conversions, bugprone-forwarding-reference-overload)
    explicit(false) Instruction(Inst&& inst) : _holder(std::forward<Inst>(inst))
    {}

    template<typename Visitor>
    auto visit(Visitor&& visitor)
    {
        return std::visit(std::forward<Visitor>(visitor), _holder);
    }

    template<typename Visitor>
    auto visit(Visitor&& visitor) const
    {
        return std::visit(std::forward<Visitor>(visitor), _holder);
    }

    [[nodiscard]] auto getOperands() const -> std::array<std::optional<Value>, 2>
    {
        return visit([]<typename T>(T const& i) {
            if constexpr (T::args == 2) {
                return std::array<std::optional<Value>, 2>{i.lhs, i.rhs};
            } else if constexpr (std::same_as<T, TruncInst> or std::same_as<T, ReturnInst>) {
                return std::array<std::optional<Value>, 2>{i.value, std::nullopt};
            } else {
                return std::array<std::optional<Value>, 2>{};
            }
        });
    }

    [[nodiscard]] auto getOperandRegisters() const -> std::array<std::optional<Register>, 2>
    {
        auto const operands = getOperands();
        auto registers      = std::array<std::optional<Register>, 2>{};
        for (auto i{0U}; i < operands.size(); ++i) {
            auto const op = operands.at(i).value_or(false);
            if (auto const* reg = std::get_if<Register>(&op); reg != nullptr) {
                registers.at(i) = *reg;
            }
        }
        return registers;
    }

    [[nodiscard]] auto getDestinationRegister() const -> std::optional<Register>
    {
        return visit([]<typename T>(T const& i) -> std::optional<Register> {
            if constexpr (requires { T::destination; }) {
                return i.destination;
            } else {
                return std::nullopt;
            }
        });
    }

private:
    using Holder = std::variant<
        NopInst,
        ConstInst,
        TruncInst,
        AddInst,
        SubInst,
        MulInst,
        DivInst,
        ModInst,
        FloatAddInst,
        FloatSubInst,
        FloatMulInst,
        FloatDivInst,
        FloatModInst,
        AndInst,
        OrInst,
        XorInst,
        ReturnInst>;

    Holder _holder;
};

}  // namespace snir
