#pragma once

#include "snir/ir/BasicBlock.hpp"
#include "snir/ir/FunctionDefinition.hpp"
#include "snir/ir/Identifier.hpp"
#include "snir/ir/Type.hpp"
#include "snir/ir/Value.hpp"

namespace snir {

struct Function
{
    explicit Function(Value value) noexcept : _value{value} {}

    [[nodiscard]] auto getValue() const -> Value { return _value; }

    [[nodiscard]] auto getType() const -> Type { return _value.get<Type>(); }

    [[nodiscard]] auto getIdentifier() const -> std::string_view
    {
        return _value.get<Identifier>().text;
    }

    [[nodiscard]] auto getArguments() const -> std::vector<ValueId> const&
    {
        return _value.get<FunctionDefinition>().args;
    }

    [[nodiscard]] auto getArguments() -> std::vector<ValueId>&
    {
        return _value.get<FunctionDefinition>().args;
    }

    [[nodiscard]] auto getBasicBlocks() const -> std::vector<BasicBlock> const&
    {
        return _value.get<FunctionDefinition>().blocks;
    }

    [[nodiscard]] auto getBasicBlocks() -> std::vector<BasicBlock>&
    {
        return _value.get<FunctionDefinition>().blocks;
    }

private:
    Value _value;
};

}  // namespace snir
