#pragma once

#include <fmt/format.h>
#include <fmt/os.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <iterator>
#include <ostream>
#include <vector>

namespace snir::lang {

struct ParseContext
{
    ParseContext() = default;

    auto addError(std::string const& msg) -> void
    {
        _errors.push_back(fmt::format("ERROR: {}", msg));
    }

    auto printErrors(std::ostream& out) const -> void
    {
        fmt::print(out, "{}", fmt::join(_errors, "\n"));
    }

    [[nodiscard]] auto hasErrors() const noexcept -> bool { return !_errors.empty(); }

private:
    std::vector<std::string> _errors;
};

}  // namespace snir::lang
