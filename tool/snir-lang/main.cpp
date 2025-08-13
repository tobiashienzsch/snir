#include "snir/core/File.hpp"
#include "snir/lang/Ast.hpp"
#include "snir/lang/Parser.hpp"

#include <cstdlib>
#include <iostream>
#include <span>

auto main(int argc, char** argv) -> int
{
    auto const args = std::span<char const* const>(argv, std::size_t(argc));
    if (args.size() != 2) {
        std::cerr << "usage: " << args[0] << " path/to/source.tcc\n";
        std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
    }

    auto src = snir::readFile(args[1]);
    auto p   = Parser{src.value()};
    auto ast = p.generateAst();

    p.printDiagnostics(std::cout);
    AstUtils::prettyPrint(std::cout, *ast);

    return EXIT_SUCCESS;
}
