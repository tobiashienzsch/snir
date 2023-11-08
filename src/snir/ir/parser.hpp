#pragma once

#include "snir/ir/instruction.hpp"
#include "snir/ir/module.hpp"

#include <string>

namespace snir {

struct Parser
{
    Parser() = default;

    [[nodiscard]] auto parseModule(std::string const& src) -> std::optional<Module>;
    [[nodiscard]] auto parseInstruction(std::string const& src) -> std::optional<Instruction>;
    [[nodiscard]] auto parseType(std::string_view src) -> std::optional<Type>;

private:
    [[nodiscard]] auto parseFunctionArgs(std::string const& src) -> std::optional<std::vector<Type>>;
    [[nodiscard]] auto parseBlocks(std::string const& src) -> std::optional<std::vector<Block>>;
    [[nodiscard]] auto parseBlock(std::string const& src) -> std::optional<Block>;
    [[nodiscard]] auto parseBinaryInst(std::string const& src) -> std::optional<Instruction>;
    [[nodiscard]] auto parseConstInst(std::string const& src) -> std::optional<ConstInst>;
    [[nodiscard]] auto parseTruncInst(std::string const& src) -> std::optional<TruncInst>;
    [[nodiscard]] auto parseReturnInst(std::string const& src) -> std::optional<ReturnInst>;
    [[nodiscard]] auto parseValue(std::string const& src, Type type) -> std::optional<Value>;
};

}  // namespace snir
