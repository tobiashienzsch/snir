#pragma once

#include "snir/ir/function.hpp"

#include <vector>

namespace snir {

struct Module
{
    std::vector<Function> functions;
};

}  // namespace snir
