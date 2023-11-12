#pragma once

#include "snir/ir/Function.hpp"
#include "snir/ir/Literal.hpp"
#include "snir/ir/ValueId.hpp"

#include <map>
#include <optional>
#include <span>

namespace snir {

struct Interpreter
{
    Interpreter() = default;

    [[nodiscard]] auto execute(Function const& func, std::span<ValueId const> args)
        -> std::optional<Literal>;

private:
    std::map<ValueId, Literal> _registers;
};

}  // namespace snir
