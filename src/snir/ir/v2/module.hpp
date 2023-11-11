#pragma once

#include "snir/ir/v2/registry.hpp"
#include "snir/ir/v2/value.hpp"

#include <vector>

namespace snir::v2 {

struct Module
{
    explicit Module(Registry& registry) : _registry{&registry} {}

    [[nodiscard]] auto getRegistry() -> Registry& { return *_registry; }

    [[nodiscard]] auto getRegistry() const -> Registry const& { return *_registry; }

    [[nodiscard]] auto getFunctions() -> std::vector<ValueId>& { return _functions; }

    [[nodiscard]] auto getFunctions() const -> std::vector<ValueId> const& { return _functions; }

private:
    Registry* _registry;
    std::vector<ValueId> _functions;
};

}  // namespace snir::v2
