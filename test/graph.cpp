#undef NDEBUG

#include "snir/graph/Graph.hpp"

#include "snir/graph/AdjacencyList.hpp"
#include "snir/ir/ValueId.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <print>

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
        std::println("node: {:c}", node);
        result.push_back(node);
    });

    assert(std::ranges::equal(result, std::array{'A', 'B', 'D', 'E', 'F', 'C'}));
}

auto testTopologicalSort() -> void
{
    using Id = int;

    auto graph = snir::Graph<Id>{};
    for (int i = 0; i < 7; i++) {
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
    auto graph = snir::Graph<int>{0, 1, 2, 3, 4, 5, 6};
    graph.connect(0, 1);
    graph.connect(1, 4);
    graph.connect(2, 3);
    graph.connect(3, 4);
    graph.connect(5, 6);

    std::println("Graph: ");
    graph.forEach([&graph](auto const& node) {
        std::print("{}: [", node);
        for (auto edge : graph.outEdges(node)) {
            std::print("{} ", edge.sink);
        }
        std::println("]");
    });

    auto stream = std::ostream_iterator<int>(std::cout, " ");

    auto const components = snir::FindComponents(graph).get();
    std::println("\nComponents: ");
    std::println("Count: {}", components.first);
    std::ranges::copy(components.second, stream);
    std::println("");

    auto const ordering = topologicalSort(graph);
    std::println("\nOrdering (TopologicalSort): ");
    std::ranges::copy(ordering, stream);
    std::println("");
}

}  // namespace

auto main() -> int
{
    testAdjacencyList();
    testTopologicalSort();
    testGraph();
    return EXIT_SUCCESS;
}
