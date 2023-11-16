#pragma once

#include <algorithm>
#include <ranges>

namespace snir::ranges {

template<std::ranges::range Range, typename T>
[[nodiscard]] auto contains(Range const& rng, T&& val) -> bool
{
    return std::ranges::find(rng, std::forward<T>(val)) != std::ranges::end(rng);
}

}  // namespace snir::ranges
