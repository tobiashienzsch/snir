#include "snir/core/file.hpp"
#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"
#include "snir/ir/instructions.hpp"
#include "snir/ir/parser.hpp"
#include "snir/ir/pretty_printer.hpp"
#include "snir/pass/optimisation_pass.hpp"
#include "snir/pass/pass_manager.hpp"

auto main() -> int
{
    auto o0 = snir::openFile("snir_O0.ll", "w");
    auto o1 = snir::openFile("snir_O1.ll", "w");

    auto opt = snir::PassManager{true};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});

    auto pm = snir::PassManager{true};
    pm.add(snir::PrettyPrinter{o0.get()});
    pm.add(std::ref(opt));
    pm.add(std::ref(opt));
    pm.add(snir::PrettyPrinter{o1.get()});

    auto ipow = snir::Function{
        .type = snir::Type::Int64,
        .name = "ipow",
        .arguments = {snir::Type::Int64, snir::Type::Int64},
        .blocks =
            {
                snir::Block{
                    snir::ConstInst{
                        .type = snir::Type::Int64,
                        .destination = snir::Register{2},
                        .value = 42,
                    },
                    snir::ReturnInst{
                        .type = snir::Type::Int64,
                        .value = snir::Register{2},
                    },
                },
            },
    };
    pm(ipow);

    return 0;
}
