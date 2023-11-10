#pragma once

#include "snir/ir/v2/module.hpp"
#include "snir/ir/v2/registry.hpp"
#include "snir/ir/v2/type.hpp"

#include <optional>
#include <string_view>

namespace snir::v2 {

struct Parser
{
    explicit Parser(Registry& registry);

    [[nodiscard]] auto readModule(std::string_view source) -> std::optional<Module>;
    [[nodiscard]] auto readInst(std::string_view source) -> std::optional<Inst>;

private:
    [[nodiscard]] auto readBasicBlocks(std::string_view source)
        -> std::optional<std::vector<BasicBlock>>;
    [[nodiscard]] auto readBasicBlock(std::string_view source) -> std::optional<BasicBlock>;

    Registry* _registry;
};

}  // namespace snir::v2
