#pragma once

#include "snir/ir/function.hpp"
#include "snir/ir/value.hpp"

#include <optional>
#include <span>

namespace snir {

struct Interpreter
{
    Interpreter() = delete;

    static auto execute(Function const& func, std::span<Value const> arguments)
        -> std::optional<Value>;
};

}  // namespace snir
