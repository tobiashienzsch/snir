#pragma once

#include "snir/ir/InstKind.hpp"
#include "snir/ir/Registry.hpp"
#include "snir/ir/Type.hpp"
#include "snir/ir/Value.hpp"
#include "snir/ir/ValueKind.hpp"

namespace snir {

struct Instruction
{
    explicit Instruction(Value value) noexcept;
    Instruction(Registry& registry, ValueId id) noexcept;

    [[nodiscard]] static auto create(Registry& reg, InstKind kind, Type type) -> Instruction;

    [[nodiscard]] auto kind() const -> InstKind;
    [[nodiscard]] auto type() const -> Type;

    [[nodiscard]] auto isTerminator() const -> bool;

    [[nodiscard]] auto asValue() const -> Value;
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    [[nodiscard]] explicit(false) operator Value() const noexcept;
    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    [[nodiscard]] explicit(false) operator ValueId() const noexcept;

private:
    Value _value;
};

}  // namespace snir
