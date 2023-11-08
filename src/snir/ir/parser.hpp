#pragma once

#include "snir/ir/instruction.hpp"
#include "snir/ir/module.hpp"

#include <string>

namespace snir {

struct Parser
{
    Parser() = default;

    [[nodiscard]] static auto parseModule(std::string const& src) -> std::optional<Module>;
    [[nodiscard]] static auto parseInstruction(std::string const& src) -> std::optional<Instruction>;
    [[nodiscard]] static auto parseType(std::string_view src) -> std::optional<Type>;

private:
    [[nodiscard]] static auto parseFunctionArgs(std::string const& src)
        -> std::optional<std::vector<Type>>;
    [[nodiscard]] static auto parseBlocks(std::string const& src)
        -> std::optional<std::vector<Block>>;
    [[nodiscard]] static auto parseBlock(std::string const& src) -> std::optional<Block>;
    [[nodiscard]] static auto parseBinaryInst(std::string const& src) -> std::optional<Instruction>;
    [[nodiscard]] static auto parseIntCmpInst(std::string const& src) -> std::optional<IntCmpInst>;
    [[nodiscard]] static auto parseConstInst(std::string const& src) -> std::optional<ConstInst>;
    [[nodiscard]] static auto parseTruncInst(std::string const& src) -> std::optional<TruncInst>;
    [[nodiscard]] static auto parseReturnInst(std::string const& src) -> std::optional<ReturnInst>;
    [[nodiscard]] static auto parseCompare(std::string const& src) -> std::optional<Compare>;
    [[nodiscard]] static auto parseValue(std::string const& src, Type type) -> std::optional<Value>;
};

}  // namespace snir
