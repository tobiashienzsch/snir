#pragma once

#include "snir/core/containers.hpp"

#include <algorithm>
#include <map>
#include <ranges>
#include <set>
#include <stack>
#include <vector>

namespace snir {

template<typename T>
using AdjacencyList = std::map<T, std::vector<T>>;

template<typename T, typename Visitor>
auto dfs(AdjacencyList<T> const& graph, T first, Visitor visitor)
{
    auto visited = std::set<T>{};
    auto stack   = std::stack<T>{};
    stack.push(first);

    while (not stack.empty()) {
        auto const current = containers::pop(stack);
        if (visited.contains(current)) {
            continue;
        }

        visitor(current);
        visited.emplace(current);

        for (auto child : graph.at(current) | std::views::reverse) {
            stack.push(child);
        }
    }
}

}  // namespace snir
