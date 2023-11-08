#pragma once

#include "snir/ir/instruction.hpp"
#include "snir/ir/module.hpp"

#include <string>

namespace snir {

struct Parser
{
    Parser() = default;

    [[nodiscard]] static auto readModule(std::string const& src) -> std::optional<Module>;
    [[nodiscard]] static auto readInstruction(std::string const& src) -> std::optional<Instruction>;
    [[nodiscard]] static auto readType(std::string_view src) -> std::optional<Type>;

private:
    [[nodiscard]] static auto readFunctionArgs(std::string const& src)
        -> std::optional<std::vector<Type>>;
    [[nodiscard]] static auto readBasicBlocks(std::string const& src)
        -> std::optional<std::vector<BasicBlock>>;

    [[nodiscard]] static auto readBasicBlock(std::string const& src) -> std::optional<BasicBlock>;
    [[nodiscard]] static auto readBinaryInst(std::string const& src) -> std::optional<Instruction>;
    [[nodiscard]] static auto readIntCmpInst(std::string const& src) -> std::optional<IntCmpInst>;
    [[nodiscard]] static auto readConstInst(std::string const& src) -> std::optional<ConstInst>;
    [[nodiscard]] static auto readTruncInst(std::string const& src) -> std::optional<TruncInst>;
    [[nodiscard]] static auto readReturnInst(std::string const& src) -> std::optional<ReturnInst>;
    [[nodiscard]] static auto readCompare(std::string const& src) -> std::optional<Compare>;
    [[nodiscard]] static auto readValue(std::string const& src, Type type) -> std::optional<Value>;
};

}  // namespace snir
