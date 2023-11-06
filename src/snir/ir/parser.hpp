#pragma once

#include "snir/ir/module.hpp"

#include <string>

namespace snir {

struct Parser
{
    Parser() = default;

    [[nodiscard]] auto operator()(std::string const& source) -> std::optional<Module>;

private:
    std::string _source;
};

}  // namespace snir
