#pragma once

#include "snir/ir/InstKind.hpp"
#include "snir/ir/Registry.hpp"
#include "snir/ir/Type.hpp"
#include "snir/ir/Value.hpp"

namespace snir {

struct Instruction
{
    explicit Instruction(Value value) noexcept : _value{value} {}

    explicit Instruction(Registry& registry, ValueId id) noexcept : _value{registry, id} {}

    [[nodiscard]] auto getValue() const -> Value { return _value; }

    [[nodiscard]] auto getKind() const -> InstKind { return _value.get<InstKind>(); }

    [[nodiscard]] auto getType() const -> Type { return _value.get<Type>(); }

    [[nodiscard]] explicit(false) operator ValueId() const noexcept { return _value; }

private:
    Value _value;
};

}  // namespace snir
