#include "snir/graph/Graph.hpp"

#include "snir/core/print.hpp"

#undef NDEBUG
#include <array>
#include <cassert>
#include <map>

auto testTopologicalSort() -> void
{
    auto graph = snir::Graph{};
    for (std::uint32_t i = 0; i < 7; i++) {
        graph.add(i);
    }

    graph.connect(0, 1);
    graph.connect(0, 2);
    graph.connect(0, 5);
    graph.connect(1, 3);
    graph.connect(1, 2);
    graph.connect(2, 3);
    graph.connect(2, 4);
    graph.connect(3, 4);
    graph.connect(5, 4);

    auto const expected = std::array<std::uint32_t, 7>{6, 0, 5, 1, 2, 3, 4};
    auto const ordering = snir::topologicalSort(graph);
    assert(std::equal(begin(ordering), end(ordering), begin(expected)));
}

auto testGraph() -> void
{
    auto graph = snir::Graph{0, 1, 2, 3, 4, 5, 6};
    graph.connect(0, 1);
    graph.connect(1, 4);
    graph.connect(2, 3);
    graph.connect(3, 4);
    graph.connect(5, 6);

    snir::println("Graph: ");
    graph.forEach([&graph](auto const& node) {
        snir::print("{}: [", node);
        for (auto edge : graph.outEdges(node)) {
            snir::print("{} ", edge.sink);
        }
        snir::println("]");
    });

    auto stream = std::ostream_iterator<std::uint32_t>(std::cout, " ");

    auto const components = snir::FindComponents(graph).get();
    snir::println("\nComponents: ");
    snir::println("Count: {}", components.first);
    std::copy(begin(components.second), end(components.second), stream);
    snir::println("");

    auto const ordering = topologicalSort(graph);
    snir::println("\nOrdering (TopologicalSort): ");
    std::copy(begin(ordering), end(ordering), stream);
    snir::println("");
}

auto main() -> int
{
    testTopologicalSort();
    testGraph();
    return EXIT_SUCCESS;
}
