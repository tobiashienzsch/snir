#pragma once

#include "snir/ir/Registry.hpp"
#include "snir/ir/ValueId.hpp"

#include <vector>

namespace snir {

struct Module
{
    explicit Module(Registry& registry) : _registry{&registry} {}

    [[nodiscard]] auto registry() -> Registry& { return *_registry; }

    [[nodiscard]] auto registry() const -> Registry const& { return *_registry; }

    [[nodiscard]] auto functions() -> std::vector<ValueId>& { return _functions; }

    [[nodiscard]] auto functions() const -> std::vector<ValueId> const& { return _functions; }

private:
    Registry* _registry;
    std::vector<ValueId> _functions;
};

}  // namespace snir
