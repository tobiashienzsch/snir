#pragma once

#include <algorithm>
#include <string>
#include <string_view>

namespace snir::strings {

[[nodiscard]] inline auto contains(std::string_view str, std::string_view sub) noexcept -> bool
{
    return str.find(sub) != std::string_view::npos;
}

[[nodiscard]] inline auto trim(std::string const& str, std::string const& whitespace = " \t")
    -> std::string
{
    auto const first = str.find_first_not_of(whitespace);
    if (first == std::string::npos) {
        return {};
    }

    auto const last  = str.find_last_not_of(whitespace);
    auto const range = last - first + 1;
    return str.substr(first, range);
}

}  // namespace snir::strings
