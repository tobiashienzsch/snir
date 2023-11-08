#pragma once

#include "snir/core/static_vector.hpp"
#include "snir/ir/compare.hpp"
#include "snir/ir/register.hpp"
#include "snir/ir/type.hpp"
#include "snir/ir/value.hpp"

#include <array>
#include <string_view>

namespace snir {

struct NopInst
{
    static constexpr auto name = std::string_view{"nop"};
    static constexpr auto args = 0;

    friend auto operator==(NopInst const& /*lhs*/, NopInst const& /*rhs*/) noexcept -> bool = default;
};

struct ReturnInst
{
    static constexpr auto name = std::string_view{"ret"};
    static constexpr auto args = 1;

    Type type;
    Value value;

    friend auto operator==(ReturnInst const& lhs, ReturnInst const& rhs) -> bool = default;
};

struct IntCmpInst
{
    static constexpr auto name = std::string_view{"icmp"};
    static constexpr auto args = 2;

    Type type;
    Compare kind;
    Register result;
    Value lhs;
    Value rhs;

    friend auto operator==(IntCmpInst const& lhs, IntCmpInst const& rhs) -> bool = default;
};

#define SNIR_INST_UNARY_OP(Id, Name)                                                                 \
    struct Id##Inst                                                                                  \
    {                                                                                                \
        static constexpr auto name = std::string_view{#Name};                                        \
        static constexpr auto args = 1;                                                              \
        Type type;                                                                                   \
        Register result;                                                                             \
        Value value;                                                                                 \
                                                                                                     \
        friend auto operator==(Id##Inst const& lhs, Id##Inst const& rhs) -> bool = default;          \
    };
#include "snir/ir/def/unary_op.def"
#undef SNIR_INST_UNARY_OP

#define SNIR_INST_BINARY_OP(Id, Name)                                                                \
    struct Id##Inst                                                                                  \
    {                                                                                                \
        static constexpr auto name = std::string_view{#Name};                                        \
        static constexpr auto args = 2;                                                              \
                                                                                                     \
        Type type;                                                                                   \
        Register result;                                                                             \
        Value lhs;                                                                                   \
        Value rhs;                                                                                   \
                                                                                                     \
        friend auto operator==(Id##Inst const& lhs, Id##Inst const& rhs) -> bool = default;          \
    };

#include "snir/ir/def/binary_op.def"
#undef SNIR_INST_BINARY_OP

struct Instruction
{
    template<typename Inst>
        requires(not std::same_as<Inst, Instruction>)
    // NOLINTNEXTLINE(hicpp-explicit-conversions, bugprone-forwarding-reference-overload)
    explicit(false) Instruction(Inst&& inst) : _holder(std::forward<Inst>(inst))
    {}

    template<typename Inst>
    [[nodiscard]] auto hasType() const noexcept -> bool
    {
        return std::holds_alternative<Inst>(_holder);
    }

    template<typename Inst>
    [[nodiscard]] auto get() const -> Inst const&
    {
        return std::get<Inst>(_holder);
    }

    template<typename Visitor>
    auto visit(Visitor&& visitor) -> decltype(auto)
    {
        return std::visit(std::forward<Visitor>(visitor), _holder);
    }

    template<typename Visitor>
    auto visit(Visitor&& visitor) const -> decltype(auto)
    {
        return std::visit(std::forward<Visitor>(visitor), _holder);
    }

    [[nodiscard]] auto getOperands() const -> StaticVector<Value, 2>
    {
        return visit([]<typename T>(T const& i) {
            if constexpr (T::args == 2) {
                return StaticVector<Value, 2>{i.lhs, i.rhs};
            } else if constexpr (requires { T::value; }) {
                return StaticVector<Value, 2>{i.value};
            } else {
                return StaticVector<Value, 2>{};
            }
        });
    }

    [[nodiscard]] auto getOperandRegisters() const -> StaticVector<Register, 2>
    {
        auto const operands = getOperands();
        auto registers      = StaticVector<Register, 2>{};
        for (auto const& op : operands) {
            if (auto const* reg = std::get_if<Register>(&op); reg != nullptr) {
                registers.push_back(*reg);
            }
        }
        return registers;
    }

    [[nodiscard]] auto getResultRegister() const -> std::optional<Register>
    {
        return visit([]<typename T>(T const& i) -> std::optional<Register> {
            if constexpr (requires { T::result; }) {
                return i.result;
            } else {
                return std::nullopt;
            }
        });
    }

    friend auto operator==(Instruction const& lhs, Instruction const& rhs) noexcept -> bool = default;

private:
    using Holder = std::variant<
        NopInst,
        ReturnInst,
        IntCmpInst,
        ConstInst,
        TruncInst,
        AddInst,
        SubInst,
        MulInst,
        DivInst,
        ModInst,
        AndInst,
        OrInst,
        XorInst,
        ShiftLeftInst,
        ShiftRightInst,
        FloatAddInst,
        FloatSubInst,
        FloatMulInst,
        FloatDivInst>;

    Holder _holder;
};

}  // namespace snir
