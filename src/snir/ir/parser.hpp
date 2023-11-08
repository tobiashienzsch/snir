#pragma once

#include "snir/ir/instruction.hpp"
#include "snir/ir/module.hpp"

#include <string>

namespace snir {

struct Parser
{
    Parser() = default;

    [[nodiscard]] auto parseModule(std::string const& source) -> std::optional<Module>;
    [[nodiscard]] auto parseInstruction(std::string const& source) -> std::optional<Instruction>;
    [[nodiscard]] auto parseType(std::string_view source) -> std::optional<Type>;
};

}  // namespace snir
