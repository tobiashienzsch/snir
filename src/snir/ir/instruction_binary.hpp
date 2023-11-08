#pragma once

#include "snir/ir/register.hpp"
#include "snir/ir/type.hpp"
#include "snir/ir/value.hpp"

#include <string_view>

namespace snir {

#define SNIR_INST_BINARY(Identifier, Name)                                                           \
    struct Identifier##Inst                                                                          \
    {                                                                                                \
        static constexpr auto name = std::string_view{#Name};                                        \
        static constexpr auto args = 2;                                                              \
        Type type;                                                                                   \
        Register result;                                                                             \
        Value lhs;                                                                                   \
        Value rhs;                                                                                   \
                                                                                                     \
        friend auto operator==(Identifier##Inst const& lhs, Identifier##Inst const& rhs) noexcept    \
            -> bool                                                                                  \
        {                                                                                            \
            return lhs.type == rhs.type and lhs.result == rhs.result and lhs.lhs == rhs.lhs          \
               and lhs.rhs == rhs.rhs;                                                               \
        }                                                                                            \
    };

#include "snir/ir/instruction_binary.def"
#undef SNIR_INST_BINARY

}  // namespace snir
