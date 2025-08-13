#include "snir/core/File.hpp"
#include "snir/lang/Ast.hpp"
#include "snir/lang/Parser.hpp"

#include <cstdlib>
#include <iostream>

auto main(int argc, char** argv) -> int
{
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " path/to/source.tcc\n";
        std::exit(EXIT_FAILURE);  // NOLINT(concurrency-mt-unsafe)
    }

    auto src = snir::readFile(argv[1]);
    auto p   = Parser{src.value()};
    auto ast = p.generateAst();

    p.printDiagnostics(std::cout);
    AstUtils::prettyPrint(std::cout, *ast);

    return EXIT_SUCCESS;
}
