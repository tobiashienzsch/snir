#pragma once

#include "snir/ir/v2/module.hpp"
#include "snir/ir/v2/type.hpp"

#include <optional>
#include <string_view>

namespace snir::v2 {

struct Parser
{
    Parser() = default;

    [[nodiscard]] auto readModule(std::string_view source) -> std::optional<Module>;
    [[nodiscard]] auto readInst(std::string_view source) -> std::optional<Inst>;

private:
    [[nodiscard]] auto readBasicBlocks(std::string_view source)
        -> std::optional<std::vector<BasicBlock>>;
    [[nodiscard]] auto readBasicBlock(std::string_view source) -> std::optional<BasicBlock>;

    Module* _current;
};

}  // namespace snir::v2
