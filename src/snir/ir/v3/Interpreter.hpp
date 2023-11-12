#pragma once

#include "snir/ir/v3/FunctionDefinition.hpp"
#include "snir/ir/v3/Literal.hpp"
#include "snir/ir/v3/Registry.hpp"
#include "snir/ir/v3/ValueId.hpp"

#include <map>
#include <optional>
#include <span>

namespace snir::v3 {

struct Interpreter
{
    explicit Interpreter(Registry& registry);

    [[nodiscard]] auto execute(FunctionDefinition const& func, std::span<ValueId const> args)
        -> std::optional<Literal>;

private:
    Registry* _registry;
    std::map<ValueId, Literal> _registers;
};

}  // namespace snir::v3
