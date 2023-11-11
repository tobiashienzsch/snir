#pragma once

#include "snir/ir/v2/registry.hpp"
#include "snir/ir/v2/type.hpp"

#include <optional>
#include <string_view>

namespace snir::v2 {

struct Parser
{
    explicit Parser(Registry& module);

    [[nodiscard]] auto read(std::string_view src) -> std::string;

private:
    [[nodiscard]] auto readArguments(std::string_view src) -> std::vector<ValueId>;
    [[nodiscard]] auto readBlocks(std::string_view src) -> std::vector<BasicBlock>;
    [[nodiscard]] auto readBlock(std::string_view src) -> BasicBlock;
    [[nodiscard]] auto readInst(std::string_view src) -> std::optional<Inst>;
    [[nodiscard]] auto readBinaryInst(std::string_view src) -> std::optional<Inst>;
    [[nodiscard]] auto readConstInst(std::string_view src) -> std::optional<Inst>;
    [[nodiscard]] auto readIntCmpInst(std::string_view src) -> std::optional<Inst>;
    [[nodiscard]] auto readTruncInst(std::string_view src) -> std::optional<Inst>;
    [[nodiscard]] auto readReturnInst(std::string_view src) -> std::optional<Inst>;
    [[nodiscard]] auto readBranchInst(std::string_view src) -> std::optional<Inst>;

    [[nodiscard]] auto getOrCreateLocal(std::string_view token, ValueKind kind) -> Value;

    Registry* _module{nullptr};
    std::map<std::string_view, ValueId> _locals;
};

}  // namespace snir::v2
