#pragma once

#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <vector>

namespace snir {

struct DirectedGraph
{
    DirectedGraph() = default;

    explicit DirectedGraph(std::size_t nodes) : _nodes{nodes}, _adjacencyMatrix(nodes * nodes, false)
    {}

    [[nodiscard]] auto nodeCount() const -> std::size_t { return _nodes; }

    [[nodiscard]] auto isConnected(std::size_t src, std::size_t dest) const -> bool
    {
        return _adjacencyMatrix.at(linearIndex(src, dest));
    }

    auto connect(std::size_t src, std::size_t dest) -> void
    {
        _adjacencyMatrix[linearIndex(src, dest)] = true;
    }

    auto disconnect(std::size_t src, std::size_t dest) -> void
    {
        _adjacencyMatrix[linearIndex(src, dest)] = false;
    }

    auto connectAll() -> void { _adjacencyMatrix.assign(_adjacencyMatrix.size(), true); }

    auto disconnectAll() -> void { _adjacencyMatrix.assign(_adjacencyMatrix.size(), false); }

private:
    [[nodiscard]] auto linearIndex(std::size_t src, std::size_t dest) const -> std::size_t
    {
        return src * nodeCount() + dest;
    }

    std::size_t _nodes{0};
    std::vector<bool> _adjacencyMatrix;
};

}  // namespace snir
