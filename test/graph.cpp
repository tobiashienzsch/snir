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
        graph.add(i, snir::Node{});
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

    auto stream         = std::ostream_iterator<std::uint32_t>(std::cout, " ");
    auto defaultHandler = [](auto x) { return x * 2.0; };

    auto graph = snir::Graph{
        {0, snir::Node{[](auto /*x*/) { return 1.0; }}},
        {1, snir::Node{defaultHandler}                },
        {2, snir::Node{[](auto /*x*/) { return 2.0; }}},
        {3, snir::Node{defaultHandler}                },
        {4, snir::Node{[](auto x) { return x; }}      },
        {5, snir::Node{[](auto x) { return x; }}      },
        {6, snir::Node{[](auto x) { return x; }}      },
    };

    graph.connect(0, 1);
    graph.connect(1, 4);
    graph.connect(2, 3);
    graph.connect(3, 4);
    graph.connect(5, 6);

    snir::println("Graph: ");
    graph.forEach([&graph](auto const& node) {
        snir::print("{}: [", node.first);
        for (auto edge : graph.outEdges(node.first)) {
            snir::print("{} ", edge.sink);
        }
        snir::println("]");
    });

    auto const components = snir::FindComponents(graph).get();
    snir::println("\nComponents: ");
    snir::println("Count: {}", components.first);
    std::copy(begin(components.second), end(components.second), stream);
    snir::println("");

    auto const ordering = topologicalSort(graph);
    snir::println("\nOrdering (TopologicalSort): ");
    std::copy(begin(ordering), end(ordering), stream);
    snir::println("");

    snir::println("\nSimulation:");
    auto buffers = std::map<std::uint32_t, double>{};
    graph.forEach([&buffers](auto const& node) { buffers.emplace(std::make_pair(node.first, 0.0)); });

    for (auto id : ordering) {
        auto const& node  = graph.node(id);
        auto const input  = buffers.at(id);
        auto const output = node.Handler(input);
        for (auto const& edge : graph.outEdges(id)) {
            buffers.at(edge.sink) += output;
        }
    }

    for (auto const& buffer : buffers) {
        snir::println("{}: {}", buffer.first, buffer.second);
    }
}

auto main() -> int
{
    testTopologicalSort();
    testGraph();
    return EXIT_SUCCESS;
}
