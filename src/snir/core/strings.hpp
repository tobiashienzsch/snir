#pragma once

#include "snir/core/exception.hpp"

#include <algorithm>
#include <charconv>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>

namespace snir::strings {

[[nodiscard]] inline auto contains(std::string_view str, std::string_view sub) noexcept -> bool
{
    return str.find(sub) != std::string_view::npos;
}

[[nodiscard]] inline auto removeSuffix(std::string_view str, std::string_view::size_type n)
    -> std::string_view
{
    str.remove_suffix(n);
    return str;
}

[[nodiscard]] inline auto trim(std::string_view str, std::string_view whitespace = " \t")
    -> std::string_view
{
    auto const first = str.find_first_not_of(whitespace);
    if (first == std::string_view::npos) {
        return {};
    }

    auto const last  = str.find_last_not_of(whitespace);
    auto const range = last - first + 1;
    return str.substr(first, range);
}

auto forEachLine(std::string_view str, auto callback) -> void
{
    if (str.empty()) {
        return;
    }

    auto first = 0;
    while (true) {
        auto const last = str.find_first_of('\n', first);
        if (last == std::string_view::npos) {
            return;
        }

        auto const line = str.substr(first, last - first);
        callback(line);
        first = last + 1;
    }
}

template<typename NumberType>
[[nodiscard]] auto tryParse(std::string_view str) -> std::optional<NumberType>
{
    auto value              = NumberType{};
    auto const* const first = str.data();
    auto const* const last  = std::next(str.data(), std::ranges::ssize(str));
    auto const result       = std::from_chars(first, last, value);
    if (result.ec == std::errc{}) {
        return value;
    }

    return std::nullopt;
}

template<typename NumberType>
[[nodiscard]] auto parse(std::string_view str) -> NumberType
{
    auto value = tryParse<NumberType>(str);
    if (not value) {
        raisef<std::invalid_argument>("failed to parse '{}' to number", str);
    }
    return *value;
}

}  // namespace snir::strings
