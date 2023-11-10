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

    [[nodiscard]] auto read(std::string_view source) -> std::optional<Module>;

private:
    Registry* _registry;
};

}  // namespace snir::v2
