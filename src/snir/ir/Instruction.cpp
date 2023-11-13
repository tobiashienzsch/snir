#include "Instruction.hpp"

#include "snir/core/ranges.hpp"

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

auto Instruction::getKind() const -> InstKind { return _value.get<InstKind>(); }

auto Instruction::getType() const -> Type { return _value.get<Type>(); }

auto Instruction::isTerminator() const -> bool
{
    return ranges::contains(std::array{InstKind::Return, InstKind::Branch}, getKind());
}

auto Instruction::getValue() const -> Value { return _value; }

Instruction::operator Value() const noexcept { return _value; }

Instruction::operator ValueId() const noexcept { return _value; }

}  // namespace snir
