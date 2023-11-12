#pragma once

#include "snir/ir/v3/Registry.hpp"
#include "snir/ir/v3/ValueId.hpp"

#include <vector>

namespace snir::v3 {

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

}  // namespace snir::v3
