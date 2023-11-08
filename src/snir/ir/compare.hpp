#pragma once

#include <cstdint>
#include <format>

namespace snir {

enum struct Compare
{
#define SNIR_INST_COMPARE_OP(Id, Name) Id,
#include "snir/ir/inst/compare_op.def"
#undef SNIR_INST_COMPARE_OP
};

}  // namespace snir

template<>
struct std::formatter<snir::Compare, char> : std::formatter<std::string_view, char>
{
    template<typename FormatContext>
    auto format(snir::Compare type, FormatContext& fc) const
    {
        static constexpr auto names = std::array{
#define SNIR_INST_COMPARE_OP(Id, Name) std::string_view{#Name},
#include "snir/ir/inst/compare_op.def"
#undef SNIR_INST_COMPARE_OP
        };

        auto str = names.at(static_cast<std::size_t>(type));
        return std::formatter<std::string_view, char>::format(str, fc);
    }
};
