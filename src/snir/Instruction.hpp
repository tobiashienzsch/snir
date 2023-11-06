#pragma once

#include "Instructions.hpp"
#include "Types.hpp"

#include <array>

namespace snir {

struct Instruction
{
    template<typename Inst>
    Instruction(Inst&& inst) : holder(std::forward<Inst>(inst))
    {}

    template<typename Visitor>
    auto visit(Visitor&& visitor)
    {
        return std::visit(std::forward<Visitor>(visitor), holder);
    }

    template<typename Visitor>
    auto visit(Visitor&& visitor) const
    {
        return std::visit(std::forward<Visitor>(visitor), holder);
    }

    auto getOperands() const -> std::array<std::optional<Operand>, 2>
    {
        return visit([]<typename T>(T const& i) {
            if constexpr (T::args == 2) {
                return std::array<std::optional<Operand>, 2>{i.lhs, i.rhs};
            } else if constexpr (std::same_as<T, TruncInst> or std::same_as<T, ReturnInst>) {
                return std::array<std::optional<Operand>, 2>{i.operand, std::nullopt};
            } else {
                return std::array<std::optional<Operand>, 2>{};
            }
        });
    }

    auto getOperandRegisters() const -> std::array<std::optional<Register>, 2>
    {
        auto const operands = getOperands();
        auto registers      = std::array<std::optional<Register>, 2>{};
        for (auto i{0U}; i < operands.size(); ++i) {
            auto const op = operands[i].value_or(false);
            if (auto* reg = std::get_if<Register>(&op); reg != nullptr) {
                registers[i] = *reg;
            }
        }
        return registers;
    }

    auto getDestinationRegister() const -> std::optional<Register>
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

    Holder holder;
};

}  // namespace snir
