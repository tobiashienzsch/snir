#pragma once

#include "snir/ir/v2/value_id.hpp"

#include <vector>

namespace snir::v2 {

struct Module
{
    std::vector<ValueId> variables;
    std::vector<ValueId> functions;
};

}  // namespace snir::v2
