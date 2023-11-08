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

private:
    [[nodiscard]] auto parseFunctionArguments(std::string const& source)
        -> std::optional<std::vector<Type>>;
    [[nodiscard]] auto parseBlocks(std::string const& source) -> std::optional<std::vector<Block>>;
    [[nodiscard]] auto parseBlock(std::string const& source) -> std::optional<Block>;
    [[nodiscard]] auto parseBinaryInst(std::string const& source) -> std::optional<Instruction>;
    [[nodiscard]] auto parseConstInst(std::string const& source) -> std::optional<ConstInst>;
    [[nodiscard]] auto parseTruncInst(std::string const& source) -> std::optional<TruncInst>;
    [[nodiscard]] auto parseReturnInst(std::string const& source) -> std::optional<ReturnInst>;
};

}  // namespace snir
