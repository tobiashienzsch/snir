#pragma once

#include "snir/ir/literal.hpp"
#include "snir/ir/operand.hpp"
#include "snir/ir/register.hpp"
#include "snir/ir/types.hpp"

#include <string_view>

namespace snir {

struct NopInst
{
    static constexpr auto name = std::string_view{"nop"};
    static constexpr auto args = 0;
};

struct ConstInst
{
    static constexpr auto name = std::string_view{"const"};
    static constexpr auto args = 1;

    Type type;
    Register destination;
    Literal literal;
};

struct TruncInst
{
    static constexpr auto name = std::string_view{"trunc"};
    static constexpr auto args = 1;

    Type type;
    Register destination;
    Operand operand;
};

struct AddInst
{
    static constexpr auto name = std::string_view{"add"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct SubInst
{
    static constexpr auto name = std::string_view{"sub"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct MulInst
{
    static constexpr auto name = std::string_view{"mul"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct DivInst
{
    static constexpr auto name = std::string_view{"div"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct ModInst
{
    static constexpr auto name = std::string_view{"mod"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct FloatAddInst
{
    static constexpr auto name = std::string_view{"fadd"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct FloatSubInst
{
    static constexpr auto name = std::string_view{"fsub"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct FloatMulInst
{
    static constexpr auto name = std::string_view{"fmul"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct FloatDivInst
{
    static constexpr auto name = std::string_view{"fdiv"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct FloatModInst
{
    static constexpr auto name = std::string_view{"fmod"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct ShiftLeftInst
{
    static constexpr auto name = std::string_view{"shl"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct ShiftRightInst
{
    static constexpr auto name = std::string_view{"shr"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct AndInst
{
    static constexpr auto name = std::string_view{"and"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct OrInst
{
    static constexpr auto name = std::string_view{"or"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct XorInst
{
    static constexpr auto name = std::string_view{"xor"};
    static constexpr auto args = 2;

    Type type;
    Register destination;
    Operand lhs;
    Operand rhs;
};

struct ReturnInst
{
    static constexpr auto name = std::string_view{"ret"};
    static constexpr auto args = 1;

    Type type;
    Operand operand;
};

}  // namespace snir
