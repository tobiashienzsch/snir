#pragma once

#include "snir/ir/v2/registry.hpp"
#include "snir/ir/v2/value_id.hpp"

#include <vector>

namespace snir::v2 {

struct Module
{
    explicit Module(Registry& registry) : _registry{registry} {}

    auto getRegistry() -> Registry& { return _registry; }

    auto getRegistry() const -> Registry const& { return _registry; }

    auto getFunctions() -> std::vector<ValueId>& { return _functions; }

    auto getFunctions() const -> std::vector<ValueId> const& { return _functions; }

private:
    std::reference_wrapper<Registry> _registry;
    std::vector<ValueId> _functions;
};

}  // namespace snir::v2
