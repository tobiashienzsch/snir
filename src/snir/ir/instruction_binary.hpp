#pragma once

#include "snir/ir/register.hpp"
#include "snir/ir/type.hpp"
#include "snir/ir/value.hpp"

#include <string_view>

namespace snir {

#define SNIR_INST_BINARY(Id, Name)                                                                   \
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

#include "snir/ir/instruction_binary.def"
#undef SNIR_INST_BINARY

}  // namespace snir
