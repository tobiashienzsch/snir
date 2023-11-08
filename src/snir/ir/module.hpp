#pragma once

#include "snir/ir/function.hpp"

#include <vector>

namespace snir {

struct Module
{
    std::vector<Function> functions;

    friend auto operator==(Module const& lhs, Module const& rhs) noexcept -> bool = default;
};

}  // namespace snir
