#pragma once

#include <cstdlib>
#include <vector>

namespace snir {

struct UndirectedGraph
{
    UndirectedGraph() = default;

    explicit UndirectedGraph(std::size_t nodes)
        : _nodes{nodes}
        , _adjacencyMatrix(matrixSize(nodes), false)
    {}

    [[nodiscard]] auto nodeCount() const -> std::size_t { return _nodes; }

    [[nodiscard]] auto isConnected(std::size_t node1, std::size_t node2) const -> bool
    {
        return _adjacencyMatrix.at(linearIndex(node1, node2));
    }

    auto connect(std::size_t node1, std::size_t node2) -> void
    {
        _adjacencyMatrix.at(linearIndex(node1, node2)) = true;
    }

    auto disconnect(std::size_t node1, std::size_t node2) -> void
    {
        _adjacencyMatrix.at(linearIndex(node1, node2)) = false;
    }

    auto connectAll() -> void { _adjacencyMatrix.assign(_adjacencyMatrix.size(), true); }

    auto disconnectAll() -> void { _adjacencyMatrix.assign(_adjacencyMatrix.size(), false); }

private:
    [[nodiscard]] static auto matrixSize(std::size_t nodes) -> std::size_t
    {
        if (nodes == 0zu) {
            return 0zu;
        }
        auto const n = nodes - 1zu;
        return (n * (n + 1zu)) / 2zu;
    }

    [[nodiscard]] static auto linearIndex(std::size_t i, std::size_t j) -> std::size_t
    {
        if (i > j) {
            std::swap(i, j);
        }
        return ((j * (j - 1)) / 2) + i;
    }

    std::size_t _nodes{0};
    std::vector<bool> _adjacencyMatrix;
};

}  // namespace snir
