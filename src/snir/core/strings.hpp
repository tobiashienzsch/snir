#pragma once

#include <algorithm>
#include <string>
#include <string_view>

namespace snir::strings {

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
