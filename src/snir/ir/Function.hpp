#pragma once

#include "snir/ir/BasicBlock.hpp"
#include "snir/ir/FunctionDefinition.hpp"
#include "snir/ir/Identifier.hpp"
#include "snir/ir/Type.hpp"
#include "snir/ir/Value.hpp"

#include <numeric>

namespace snir {

struct Function
{
    explicit Function(Value value) noexcept : _value{value} {}

    Function(Registry& reg, ValueId id) noexcept : _value{reg, id} {}

    [[nodiscard]] static auto create(Registry& reg, Type type) -> Function
    {
        auto func = Value{reg, reg.create()};
        func.emplace<ValueKind>(ValueKind::Function);
        func.emplace<Type>(type);
        return Function{func};
    }

    [[nodiscard]] auto type() const -> Type { return _value.get<Type>(); }

    auto identifier(std::string_view text) const -> void
    {
        _value.emplace_or_replace<Identifier>(std::string{text});
    }

    [[nodiscard]] auto identifier() const -> std::string_view
    {
        return _value.get<Identifier>().text;
    }

    [[nodiscard]] auto arguments() const -> std::vector<ValueId> const&
    {
        return _value.get<FunctionDefinition>().args;
    }

    [[nodiscard]] auto arguments() -> std::vector<ValueId>&
    {
        return _value.get<FunctionDefinition>().args;
    }

    [[nodiscard]] auto basicBlocks() const -> std::vector<BasicBlock> const&
    {
        return _value.get<FunctionDefinition>().blocks;
    }

    [[nodiscard]] auto basicBlocks() -> std::vector<BasicBlock>&
    {
        return _value.get<FunctionDefinition>().blocks;
    }

    [[nodiscard]] auto numInstructions() const -> std::size_t
    {
        auto const& blocks = basicBlocks();
        return std::accumulate(blocks.begin(), blocks.end(), 0zu, [](auto sum, auto const& block) {
            return sum + block.instructions.size();
        });
    }

    [[nodiscard]] auto asValue() const noexcept -> Value { return _value; }

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    [[nodiscard]] explicit(false) operator Value() const noexcept { return _value; }

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    [[nodiscard]] explicit(false) operator ValueId() const noexcept { return _value; }

private:
    Value _value;
};

}  // namespace snir
