#pragma once

#include "snir/ir/register.hpp"
#include "snir/ir/type.hpp"
#include "snir/ir/value.hpp"

#include <string_view>

namespace snir {

struct NopInst
{
    static constexpr auto name = std::string_view{"nop"};
    static constexpr auto args = 0;

    friend auto operator==(NopInst const& lhs, NopInst const& rhs) noexcept -> bool { return true; }
};

struct ReturnInst
{
    static constexpr auto name = std::string_view{"ret"};
    static constexpr auto args = 1;

    Type type;
    Value value;

    friend auto operator==(ReturnInst const& lhs, ReturnInst const& rhs) noexcept -> bool
    {
        return lhs.type == rhs.type and lhs.value == rhs.value;
    }
};

#define SNIR_INST_UNARY(Identifier, Name)                                                            \
    struct Identifier##Inst                                                                          \
    {                                                                                                \
        static constexpr auto name = std::string_view{#Name};                                        \
        static constexpr auto args = 1;                                                              \
        Type type;                                                                                   \
        Register result;                                                                             \
        Value value;                                                                                 \
                                                                                                     \
        friend auto operator==(Identifier##Inst const& lhs, Identifier##Inst const& rhs) noexcept    \
            -> bool                                                                                  \
        {                                                                                            \
            return lhs.type == rhs.type and lhs.result == rhs.result and lhs.value == rhs.value;     \
        }                                                                                            \
    };
#include "snir/ir/instruction_unary.def"
#undef SNIR_INST_UNARY

}  // namespace snir
