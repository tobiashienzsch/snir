#pragma once

#include "snir/core/exception.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <vector>

namespace snir {

template<typename NodeType>
struct Graph
{
    using Node = NodeType;

    struct Edge
    {
        NodeType source;
        NodeType sink;
    };

    Graph(std::initializer_list<Node> ilist) : _nodes(ilist) {}

    auto add(NodeType id) -> void
    {
        if (idExists(id)) {
            raisef<std::runtime_error>("node '{}' already exists in graph", id);
        }

        _nodes.emplace_back(id);
        sortNodes();
        return;
    }

    auto connect(NodeType source, NodeType sink) -> void
    {
        _edges.emplace_back(Edge{source, sink});
        sortEdges();
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t { return _nodes.size(); }

    [[nodiscard]] auto getInEdges(NodeType id) const -> std::vector<Edge>
    {
        auto result = std::vector<Edge>{};
        std::ranges::copy_if(_edges, std::back_inserter(result), [id](Edge edge) {
            return edge.sink == id;
        });
        return result;
    }

    [[nodiscard]] auto getOutEdges(NodeType id) const -> std::vector<Edge>
    {
        auto result = std::vector<Edge>{};
        std::ranges::copy_if(_edges, std::back_inserter(result), [id](Edge edge) {
            return edge.source == id;
        });
        return result;
    }

    template<typename Func>
    auto forEach(Func func) const -> void
    {
        for (auto const& node : _nodes) {
            func(node);
        }
    }

private:
    [[nodiscard]] auto idExists(NodeType id) const noexcept -> bool
    {
        return std::ranges::find(_nodes, id) != _nodes.end();
    }

    auto sortNodes() -> void { std::ranges::sort(_nodes); }

    auto sortEdges() -> void { std::ranges::sort(_edges, {}, &Edge::source); }

    std::vector<Node> _nodes;
    std::vector<Edge> _edges;
};

/*
 * @brief Performs a depth first search on the graph to give us the topological
 * ordering we want. Instead of maintaining a stack of the nodes we see we
 * simply place them inside the ordering vector in reverse order for simplicity.
 */
template<typename NodeType>
[[nodiscard]] auto depthFirstSearch(
    NodeType orderIndex,
    NodeType currentNodeID,
    std::vector<bool>& visited,
    std::vector<NodeType>& ordering,
    Graph<NodeType> const& graph
) -> NodeType
{

    visited[currentNodeID] = true;

    if (auto const& edges = graph.getOutEdges(currentNodeID); !edges.empty()) {
        for (auto const& edge : edges) {
            if (!visited[edge.sink]) {
                orderIndex = depthFirstSearch(orderIndex, edge.sink, visited, ordering, graph);
            }
        }
    }

    ordering[orderIndex] = currentNodeID;
    return orderIndex - 1;
}

/*
 * @brief Finds a topological ordering of the nodes in a Directed Acyclic Graph
 * (DAG). The input to this function is an adjacency list for a graph.
 */
template<typename NodeType>
[[nodiscard]] auto topologicalSort(Graph<NodeType> const& graph) -> std::vector<NodeType>
{
    auto const size = graph.size();
    auto ordering   = std::vector<NodeType>(size);
    auto visited    = std::vector<bool>(size);

    auto i = size - 1;
    for (NodeType at = 0; at < size; at++) {
        if (!visited[at]) {
            i = depthFirstSearch(static_cast<NodeType>(i), at, visited, ordering, graph);
        }
    }

    return ordering;
}

template<typename NodeType>
struct FindComponents
{
    explicit FindComponents(Graph<NodeType> const& graph)
        : _graph(graph)
        , _size(static_cast<NodeType>(_graph.size()))
        , _components(_size)
        , _visited(_size)
    {
        assert(_components.size() == _size);
        assert(_visited.size() == _size);

        for (NodeType i = 0; i < _size; ++i) {
            if (!_visited[i]) {
                ++_count;
                dfs(i);
            }
        }
    }

    [[nodiscard]] auto get() const -> std::pair<NodeType, std::vector<NodeType>>
    {
        return std::make_pair(_count, _components);
    }

private:
    auto dfs(NodeType currentNode) -> void
    {
        _visited[currentNode]    = true;
        _components[currentNode] = _count;

        if (auto const& edges = _graph.getOutEdges(currentNode); !edges.empty()) {
            for (auto const& edge : edges) {
                if (!_visited[edge.sink]) {
                    dfs(edge.sink);
                }
            }
        }
    }

    Graph<NodeType> const& _graph;
    NodeType _size;
    NodeType _count{0};
    std::vector<NodeType> _components;
    std::vector<bool> _visited;
};

}  // namespace snir
