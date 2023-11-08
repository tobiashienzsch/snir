#include "snir/core/file.hpp"
#include "snir/core/strings.hpp"
#include "snir/ir/parser.hpp"
#include "snir/ir/pass/dead_store_elimination.hpp"
#include "snir/ir/pass/remove_empty_block.hpp"
#include "snir/ir/pass/remove_nop.hpp"
#include "snir/ir/pass_manager.hpp"
#include "snir/ir/pretty_printer.hpp"
#include "snir/vm/interpreter.hpp"

#include <chrono>
#include <fstream>

namespace {
struct Arguments
{
    std::filesystem::path input;
    std::filesystem::path output;
    int opt{1};
    bool verbose{false};
};

[[nodiscard]] auto parseArguments(int argc, char const* const* argv) -> std::optional<Arguments>
{
    auto args = Arguments{};
    for (auto i{1}; i < argc; ++i) {
        if (snir::strings::trim(argv[i]) == std::string_view{"-v"}) {
            args.verbose = true;
            continue;
        }
        if (snir::strings::contains(argv[i], "-O")) {
            args.opt = std::stoi(std::string(argv[i]).substr(2));
            continue;
        }
        if (snir::strings::contains(argv[i], "-o")) {
            args.output = argv[++i];
            continue;
        }
    }

    args.input = argv[argc - 1];
    return args;
};

}  // namespace

auto main(int argc, char const* const* argv) -> int
{
    // Parse arguments
    auto args = parseArguments(argc, argv);
    if (not args) {
        snir::println("Usage:\nsnex-opt -v -O[0,1,2]");
        return EXIT_FAILURE;
    }

    // Parse source
    auto src    = snir::readFile(args->input).value();
    auto parser = snir::Parser{};
    auto module = snir::Module{};
    {
        auto const start = std::chrono::steady_clock::now();
        module           = parser.parseModule(src).value();
        if (args->verbose) {
            auto const stop  = std::chrono::steady_clock::now();
            auto const delta = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            snir::println("parse: {}", delta);
        }
    }

    // Add passes
    auto pm  = snir::PassManager{args->verbose, std::cout};
    auto opt = snir::PassManager{args->verbose, std::cout};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});
    if (args->opt > 0) {
        pm.add(std::ref(opt));
    }
    if (args->opt > 1) {
        pm.add(std::ref(opt));
    }

    // Print optimized source
    auto out = std::fstream(args->output, std::ios::out);
    pm.add(snir::PrettyPrinter{out});

    // Run passes
    pm(module);

    // Execute
    auto result = snir::Interpreter::execute(module.functions.at(0), {});
    snir::println("{}", result.value());

    return 0;
}
