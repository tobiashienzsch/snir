#pragma once

#include "snir/ir/v3/ValueId.hpp"

#include <optional>

namespace snir::v3 {

struct Branch
{
    ValueId iftrue;
    std::optional<ValueId> iffalse;
    std::optional<ValueId> condition;
};

}  // namespace snir::v3
