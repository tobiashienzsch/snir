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

    friend auto operator==(NopInst const& lhs, NopInst const& rhs) noexcept -> bool = default;
};

struct ReturnInst
{
    static constexpr auto name = std::string_view{"ret"};
    static constexpr auto args = 1;

    Type type;
    Value value;

    friend auto operator==(ReturnInst const& lhs, ReturnInst const& rhs) -> bool = default;
};

#define SNIR_INST_UNARY(Id, Name)                                                                    \
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
#include "snir/ir/instruction_unary.def"
#undef SNIR_INST_UNARY

}  // namespace snir
