#pragma once

#include "Function.hpp"

#include <string>

namespace snir {

struct Module
{
    std::vector<Function> functions;
};

struct Parser
{
    Parser() = default;

    [[nodiscard]] auto operator()(std::string const& source) -> std::optional<Module>;

private:
    std::string _source;
};

}  // namespace snir
