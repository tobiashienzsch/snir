#pragma once

#include "snir/ir/BasicBlock.hpp"
#include "snir/ir/Module.hpp"
#include "snir/ir/Registry.hpp"
#include "snir/ir/Type.hpp"
#include "snir/ir/Value.hpp"
#include "snir/ir/ValueKind.hpp"

#include <map>
#include <optional>
#include <string_view>

namespace snir::v3 {

struct Parser
{
    explicit Parser(Registry& registry);

    [[nodiscard]] auto read(std::string_view source) -> std::optional<Module>;

private:
    [[nodiscard]] auto readArguments(std::string_view source) -> std::vector<ValueId>;
    [[nodiscard]] auto readBlocks(std::string_view source) -> std::vector<BasicBlock>;
    [[nodiscard]] auto readBlock(std::string_view source) -> BasicBlock;
    [[nodiscard]] auto readInst(std::string_view source) -> std::optional<ValueId>;
    [[nodiscard]] auto readBinaryInst(std::string_view source) -> std::optional<ValueId>;
    [[nodiscard]] auto readIntCmpInst(std::string_view source) -> std::optional<ValueId>;
    [[nodiscard]] auto readTruncInst(std::string_view source) -> std::optional<ValueId>;
    [[nodiscard]] auto readReturnInst(std::string_view source) -> std::optional<ValueId>;
    [[nodiscard]] auto readBranchInst(std::string_view source) -> std::optional<ValueId>;
    [[nodiscard]] auto readConstInst(std::string_view source) -> std::optional<ValueId>;

    [[nodiscard]] auto getOrCreateLocal(std::string_view token, ValueKind kind) -> Value;

    Registry* _registry{nullptr};
    std::map<std::string_view, ValueId> _locals;
};

}  // namespace snir::v3
