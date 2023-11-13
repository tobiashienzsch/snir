#pragma once

#include "snir/ir/Registry.hpp"
#include "snir/ir/ValueId.hpp"
#include "snir/ir/ValueKind.hpp"

#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

namespace snir {

using Value = entt::handle;

[[nodiscard]] inline auto createValue(Registry& reg, ValueKind kind) -> Value
{
    auto val = Value{reg, reg.create()};
    val.emplace<ValueKind>(kind);
    return val;
}

}  // namespace snir
