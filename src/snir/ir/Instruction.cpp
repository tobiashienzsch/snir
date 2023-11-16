#include "Instruction.hpp"

#include "snir/core/Ranges.hpp"

namespace snir {

Instruction::Instruction(Value value) noexcept : _value{value} {}

Instruction::Instruction(Registry& registry, ValueId id) noexcept : _value{registry, id} {}

auto Instruction::create(Registry& reg, InstKind kind, Type type) -> Instruction
{
    auto inst = reg.create();
    reg.emplace<ValueKind>(inst, ValueKind::Instruction);
    reg.emplace<InstKind>(inst, kind);
    reg.emplace<Type>(inst, type);
    return Instruction{reg, inst};
}

auto Instruction::kind() const -> InstKind { return _value.get<InstKind>(); }

auto Instruction::type() const -> Type { return _value.get<Type>(); }

auto Instruction::isTerminator() const -> bool
{
    return ranges::contains(std::array{InstKind::Return, InstKind::Branch}, kind());
}

auto Instruction::asValue() const -> Value { return _value; }

Instruction::operator Value() const noexcept { return _value; }

Instruction::operator ValueId() const noexcept { return _value; }

}  // namespace snir
