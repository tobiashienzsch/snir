#include "snir/ir/CompareKind.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Identifier.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Interpreter.hpp"
#include "snir/ir/Literal.hpp"
#include "snir/ir/Parser.hpp"
#include "snir/ir/pass/DeadStoreElimination.hpp"
#include "snir/ir/pass/RemoveEmptyBlock.hpp"
#include "snir/ir/pass/RemoveNop.hpp"
#include "snir/ir/PassManager.hpp"
#include "snir/ir/Printer.hpp"
#include "snir/ir/Registry.hpp"
#include "snir/ir/Type.hpp"

#include "snir/core/file.hpp"
#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

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

[[nodiscard]] auto parseArguments(std::span<char const* const> arguments) -> std::optional<Arguments>
{
    auto args = Arguments{};
    for (auto i{1U}; i < arguments.size(); ++i) {
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

namespace ir = snir::v3;

auto main(int argc, char const* const* argv) -> int
{
    // Parse arguments
    auto args = parseArguments(std::span<char const* const>(argv, std::size_t(argc)));
    if (not args) {
        snir::println("Usage:\nsnex-opt -v -O[0,1,2]");
        return EXIT_FAILURE;
    }

    auto registry = ir::Registry{};
    auto parser   = ir::Parser{registry};
    auto source   = snir::readFile(args->input).value();

    auto module  = parser.read(source);
    auto funcId  = module->getFunctions().at(0);
    auto funcVal = ir::Value{registry, funcId};
    auto func    = ir::Function{funcVal};

    // Add passes
    auto pm  = ir::PassManager{args->verbose, std::cout};
    auto opt = ir::PassManager{args->verbose, std::cout};
    opt.add(ir::DeadStoreElimination{});
    opt.add(ir::RemoveNop{});
    opt.add(ir::RemoveEmptyBlock{});
    if (args->opt > 0) {
        pm.add(std::ref(opt));
    }
    if (args->opt > 1) {
        pm.add(std::ref(opt));
    }

    // Print optimized source
    auto out = std::fstream(args->output, std::ios::out);
    pm.add(ir::Printer{out});

    // Run passes
    pm(*module);

    auto vm     = ir::Interpreter{};
    auto result = vm.execute(func, {});
    snir::println("; return: {} as {}", *result, func.getType());

    return 0;
}
