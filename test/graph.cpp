#undef NDEBUG

#include "snir/graph/Graph.hpp"
#include "snir/graph/AdjacencyList.hpp"
#include "snir/graph/DirectedGraph.hpp"
#include "snir/graph/UndirectedGraph.hpp"

#include "fmt/os.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>

namespace {

auto testAdjacencyList() -> void
{
    auto const graph = snir::AdjacencyList<char>{
        {'A', {'B', 'C'}},
        {'B', {'D', 'E'}},
        {'C', {'F'}     },
        {'D', {}        },
        {'E', {'F'}     },
        {'F', {}        },
    };
    auto result = std::vector<char>{};
    snir::dfs(graph, 'A', [&](auto node) {
        fmt::println("node: {:c}", node);
        result.push_back(node);
    });

    assert(std::ranges::equal(result, std::array{'A', 'B', 'D', 'E', 'F', 'C'}));
}

auto testTopologicalSort() -> void
{
    using Id = unsigned;

    auto graph = snir::Graph<Id>{};
    for (unsigned i = 0; i < 7; i++) {
        graph.add(Id(i));
    }

    graph.connect(Id(0), Id(1));
    graph.connect(Id(0), Id(2));
    graph.connect(Id(0), Id(5));
    graph.connect(Id(1), Id(3));
    graph.connect(Id(1), Id(2));
    graph.connect(Id(2), Id(3));
    graph.connect(Id(2), Id(4));
    graph.connect(Id(3), Id(4));
    graph.connect(Id(5), Id(4));

    auto const expected = std::array<Id, 7>{Id(6), Id(0), Id(5), Id(1), Id(2), Id(3), Id(4)};
    auto const ordering = snir::topologicalSort(graph);
    assert(std::ranges::equal(ordering, expected));
}

auto testGraph() -> void
{
    auto graph = snir::Graph<unsigned>{0, 1, 2, 3, 4, 5, 6};
    graph.connect(0, 1);
    graph.connect(1, 4);
    graph.connect(2, 3);
    graph.connect(3, 4);
    graph.connect(5, 6);

    fmt::println("Graph: ");
    graph.forEach([&graph](auto const& node) {
        fmt::print("{}: [", node);
        for (auto edge : graph.outEdges(node)) {
            fmt::print("{} ", edge.sink);
        }
        fmt::println("]");
    });

    auto stream = std::ostream_iterator<unsigned>(std::cout, " ");

    auto const ordering = topologicalSort(graph);
    fmt::println("\nOrdering (TopologicalSort): ");
    std::ranges::copy(ordering, stream);
    fmt::println("");
}

auto testUndirectedGraph() -> void
{
    auto graph = snir::UndirectedGraph{4zu};

    graph.connect(0, 1);
    graph.connect(0, 2);
    assert(graph.isConnected(0, 1));
    assert(graph.isConnected(1, 0));
    assert(not graph.isConnected(0, 3));
    assert(not graph.isConnected(3, 0));

    graph.disconnect(0, 1);
    graph.disconnect(0, 2);
    assert(not graph.isConnected(0, 1));
    assert(not graph.isConnected(1, 0));
    assert(not graph.isConnected(0, 3));
    assert(not graph.isConnected(3, 0));

    graph.connectAll();
    assert(graph.isConnected(0, 1));
    assert(graph.isConnected(1, 0));
    assert(graph.isConnected(0, 3));
    assert(graph.isConnected(3, 0));
}

auto testDirectedGraph() -> void
{
    auto graph = snir::DirectedGraph{3zu};
    graph.connect(0, 1);
    assert(graph.isConnected(0, 1));
    assert(not graph.isConnected(1, 0));
    assert(not graph.isConnected(2, 0));
    assert(not graph.isConnected(0, 2));

    graph.disconnect(0, 1);
    graph.disconnect(0, 2);
    assert(not graph.isConnected(0, 1));
    assert(not graph.isConnected(0, 2));

    graph.connectAll();
    assert(graph.isConnected(0, 0));
    assert(graph.isConnected(0, 1));
    assert(graph.isConnected(0, 2));
    assert(graph.isConnected(0, 0));
    assert(graph.isConnected(1, 0));
    assert(graph.isConnected(2, 0));

    graph.disconnectAll();
    assert(not graph.isConnected(0, 0));
    assert(not graph.isConnected(0, 1));
    assert(not graph.isConnected(0, 2));
    assert(not graph.isConnected(0, 0));
    assert(not graph.isConnected(1, 0));
    assert(not graph.isConnected(2, 0));
}

}  // namespace

auto main() -> int
{
    testAdjacencyList();
    testTopologicalSort();
    testGraph();
    testUndirectedGraph();
    testDirectedGraph();
    return EXIT_SUCCESS;
}
