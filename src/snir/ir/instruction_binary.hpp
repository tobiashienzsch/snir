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
    };

#include "snir/ir/instruction_binary.def"
#undef SNIR_INST_BINARY

}  // namespace snir
