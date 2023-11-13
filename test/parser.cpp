#include "snir/ir/Parser.hpp"
#include "snir/ir/Branch.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/InstKind.hpp"

#include "snir/core/file.hpp"
#include "snir/core/print.hpp"

#undef NDEBUG
#include <cassert>

auto testParser() -> void
{
    auto registry = snir::Registry{};
    auto parser   = snir::Parser{registry};
    auto inst     = registry.view<snir::InstKind>();
    auto branch   = registry.view<snir::Branch>();

    auto const* file = "./test/files/i64_blocks.ll";
    snir::println("; {}", file);

    auto const source = snir::readFile(file).value();
    auto const module = parser.read(source);
    assert(module.has_value());

    auto const func    = snir::Function{snir::Value(registry, module->getFunctions().at(0))};
    auto const& blocks = func.getBasicBlocks();
    assert(blocks.size() == 4);

    auto const terminal       = blocks.at(0).instructions.back();
    auto const [terminalKind] = inst.get(terminal);
    assert(terminalKind == snir::InstKind::Branch);

    auto const [br] = branch.get(terminal);
    assert(br.iftrue == blocks.at(1).label);
}

auto main() -> int
{
    testParser();
    return EXIT_SUCCESS;
}
