#pragma once

#include "snir/ir/v3/BasicBlock.hpp"
#include "snir/ir/v3/FunctionDefinition.hpp"
#include "snir/ir/v3/Value.hpp"

namespace snir::v3 {

struct Function
{
    explicit Function(Value value) noexcept : _value{value} {}

    [[nodiscard]] auto getValue() const -> Value { return _value; }

    [[nodiscard]] auto getArguments() const -> std::vector<ValueId> const&
    {
        return _value.get<FunctionDefinition>().args;
    }

    [[nodiscard]] auto getBasicBlocks() const -> std::vector<BasicBlock> const&
    {
        return _value.get<FunctionDefinition>().blocks;
    }

private:
    Value _value;
};

}  // namespace snir::v3
