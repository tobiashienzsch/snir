#include "snir/core/File.hpp"
#include "snir/core/Strings.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Interpreter.hpp"
#include "snir/ir/Parser.hpp"
#include "snir/ir/pass/DeadStoreElimination.hpp"
#include "snir/ir/pass/RemoveEmptyBlock.hpp"
#include "snir/ir/pass/RemoveNop.hpp"
#include "snir/ir/PassManager.hpp"
#include "snir/ir/Printer.hpp"
#include "snir/ir/Registry.hpp"

#include "fmt/os.h"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <optional>
#include <span>

namespace {
struct Arguments
{
    std::filesystem::path input;
    std::filesystem::path output;
    int opt{1};
    bool verbose{false};
};

[[nodiscard]] auto parseArguments(std::span<char const* const> arguments) -> std::optional<Arguments>
{
    auto args = Arguments{};
    for (auto i{1zu}; i < arguments.size(); ++i) {
        if (snir::strings::trim(arguments[i]) == std::string_view{"-v"}) {
            args.verbose = true;
            continue;
        }
        if (snir::strings::contains(arguments[i], "-O")) {
            args.opt = snir::strings::parse<int>(std::string_view(arguments[i]).substr(2));
            continue;
        }
        if (snir::strings::contains(arguments[i], "-o")) {
            args.output = arguments[++i];
            continue;
        }
    }

    args.input = arguments.back();
    return args;
}

}  // namespace

auto main(int argc, char const* const* argv) -> int
try {
    // Parse arguments
    auto args = parseArguments(std::span<char const* const>(argv, std::size_t(argc)));
    if (not args) {
        fmt::println(stderr, "Usage:\nsnir-opt -v -O[0,1,2]");
        return EXIT_FAILURE;
    }

    if (not std::filesystem::is_regular_file(args->input)) {
        fmt::println(stderr, "Input file does not exist: {}", args->input.string());
        return EXIT_FAILURE;
    }

    auto registry = snir::Registry{};
    auto parser   = snir::Parser{registry};
    auto source   = snir::readFile(args->input).value();

    auto module  = parser.read(source);
    auto funcId  = module.functions().at(0);
    auto funcVal = snir::Value{registry, funcId};
    auto func    = snir::Function{funcVal};

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
    if (not args->output.empty()) {
        pm.add(snir::Printer{out});
    }

    // Run passes
    pm(module);

    if (func.arguments().empty()) {
        auto vm     = snir::Interpreter{};
        auto result = vm.execute(func, {});
        fmt::println("; return: {} as {}", result.value(), func.type());
    }

    return EXIT_SUCCESS;
} catch (std::exception const& e) {
    fmt::println(stderr, "Exception in main(): {}", e.what());
    return EXIT_FAILURE;
} catch (...) {
    fmt::println(stderr, "Unkown exception in main()");
    return EXIT_FAILURE;
}
