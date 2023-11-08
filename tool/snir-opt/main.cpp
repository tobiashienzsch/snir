#include "snir/core/file.hpp"
#include "snir/ir/parser.hpp"
#include "snir/ir/pass/dead_store_elimination.hpp"
#include "snir/ir/pass/remove_empty_block.hpp"
#include "snir/ir/pass/remove_nop.hpp"
#include "snir/ir/pass_manager.hpp"
#include "snir/ir/pretty_printer.hpp"

#include <fstream>

auto main() -> int
{
    auto src    = snir::readFile("./test/files/develop.ll").value();
    auto parser = snir::Parser{};
    auto module = parser.parseModule(src).value();

    auto opt = snir::PassManager{true};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});

    auto pm = snir::PassManager{true};
    pm.add(std::ref(opt));

    auto out = std::fstream("./out.ll", std::ios::out);
    pm.add(snir::PrettyPrinter{out});
    pm(module);

    return 0;
}
