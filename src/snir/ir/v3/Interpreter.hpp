#pragma once

#include "snir/ir/v3/Function.hpp"
#include "snir/ir/v3/Literal.hpp"
#include "snir/ir/v3/ValueId.hpp"

#include <map>
#include <optional>
#include <span>

namespace snir::v3 {

struct Interpreter
{
    Interpreter() = default;

    [[nodiscard]] auto execute(Function const& func, std::span<ValueId const> args)
        -> std::optional<Literal>;

private:
    std::map<ValueId, Literal> _registers;
};

}  // namespace snir::v3
