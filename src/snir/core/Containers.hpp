#pragma once

#include <stack>

namespace snir::containers {

template<typename T>
[[nodiscard]] auto pop(std::stack<T>& stack) -> T
{
    auto val = stack.top();
    stack.pop();
    return val;
}

}  // namespace snir::containers
