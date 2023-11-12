#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <vector>

namespace snir {

struct Graph
{
    using Node = std::uint32_t;

    struct Edge
    {
        std::uint32_t source;
        std::uint32_t sink;
    };

    Graph(std::initializer_list<Node> ilist) : _nodes(ilist) {}

    auto add(std::uint32_t id) -> bool
    {
        if (idExists(id)) {
            return false;
        }
        _nodes.emplace_back(id);
        sortNodes();
        return true;
    }

    auto connect(std::uint32_t source, std::uint32_t sink) -> void
    {
        _edges.emplace_back(Edge{source, sink});
        sortEdges();
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t { return _nodes.size(); }

    [[nodiscard]] auto inEdges(std::uint32_t nodeID) const -> std::vector<Edge>
    {
        auto result = std::vector<Edge>{};
        std::copy_if(
            begin(_edges),
            end(_edges),
            std::back_inserter(result),
            [nodeID](auto const& edge) { return edge.sink == nodeID; }
        );
        return result;
    }

    [[nodiscard]] auto outEdges(std::uint32_t nodeID) const -> std::vector<Edge>
    {
        auto result = std::vector<Edge>{};
        std::copy_if(
            begin(_edges),
            end(_edges),
            std::back_inserter(result),
            [nodeID](auto const& edge) { return edge.source == nodeID; }
        );
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
    [[nodiscard]] auto idExists(std::uint32_t id) const noexcept -> bool
    {
        return std::find(begin(_nodes), end(_nodes), id) != end(_nodes);
    }

    auto sortNodes() -> void { std::sort(begin(_nodes), end(_nodes)); }

    auto sortEdges() -> void
    {
        std::sort(begin(_edges), end(_edges), [](auto const& lhs, auto const& rhs) {
            return lhs.source < rhs.source;
        });
    }

    std::vector<Node> _nodes;
    std::vector<Edge> _edges;
};

/*
 * @brief Performs a depth first search on the graph to give us the topological
 * ordering we want. Instead of maintaining a stack of the nodes we see we
 * simply place them inside the ordering vector in reverse order for simplicity.
 */
inline auto depthFirstSearch(
    std::uint32_t orderIndex,
    std::uint32_t currentNodeID,
    std::vector<bool>& visited,
    std::vector<std::uint32_t>& ordering,
    Graph const& graph
) -> std::uint32_t
{

    visited[currentNodeID] = true;

    if (auto const& edges = graph.outEdges(currentNodeID); !edges.empty()) {
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
inline auto topologicalSort(Graph const& graph) -> std::vector<std::uint32_t>
{
    auto const size = graph.size();
    auto ordering   = std::vector<std::uint32_t>(size);
    auto visited    = std::vector<bool>(size);

    auto i = size - 1;
    for (std::uint32_t at = 0; at < size; at++) {
        if (!visited[at]) {
            i = depthFirstSearch(static_cast<std::uint32_t>(i), at, visited, ordering, graph);
        }
    }

    return ordering;
}

struct FindComponents
{
    explicit FindComponents(Graph const& graph)
        : _graph(graph)
        , _size(static_cast<std::uint32_t>(_graph.size()))
        , _components(_size)
        , _visited(_size)
    {
        assert(_components.size() == _size);
        assert(_visited.size() == _size);

        for (std::uint32_t i = 0; i < _size; ++i) {
            if (!_visited[i]) {
                ++count_;
                dfs(i);
            }
        }
    }

    [[nodiscard]] auto get() const -> std::pair<std::uint32_t, std::vector<std::uint32_t>>
    {
        return std::make_pair(count_, _components);
    }

private:
    auto dfs(std::uint32_t currentNode) -> void
    {
        _visited[currentNode]    = true;
        _components[currentNode] = count_;

        if (auto const& edges = _graph.outEdges(currentNode); !edges.empty()) {
            for (auto const& edge : edges) {
                if (!_visited[edge.sink]) {
                    dfs(edge.sink);
                }
            }
        }
    }

    Graph const& _graph;
    std::uint32_t _size;
    std::uint32_t count_{0};
    std::vector<std::uint32_t> _components;
    std::vector<bool> _visited;
};

}  // namespace snir
