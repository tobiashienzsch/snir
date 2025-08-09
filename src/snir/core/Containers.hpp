#pragma once

#include <stack>

namespace snir::containers {

template<typename T, typename Container>
[[nodiscard]] auto pop(std::stack<T, Container>& stack) -> T
{
    auto val = stack.top();
    stack.pop();
    return val;
}

}  // namespace snir::containers
