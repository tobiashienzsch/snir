#pragma once

#include "snir/ir/ValueId.hpp"

#include <optional>

namespace snir {

struct Branch
{
    ValueId iftrue{};
    std::optional<ValueId> iffalse;
    std::optional<ValueId> condition;
};

}  // namespace snir
