#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"
#include "snir/ir/pass/dead_store_elimination.hpp"
#include "snir/ir/pass/remove_empty_block.hpp"
#include "snir/ir/pass/remove_nop.hpp"
#include "snir/ir/pass_manager.hpp"
#include "snir/ir/pretty_printer.hpp"

#include <fstream>

auto main() -> int
{
    auto o0 = std::fstream("snir_O0.ll", std::ios::out);
    auto o1 = std::fstream("snir_O1.ll", std::ios::out);

    auto opt = snir::PassManager{true};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});

    auto pm = snir::PassManager{true};
    pm.add(snir::PrettyPrinter{o0});
    pm.add(std::ref(opt));
    pm.add(std::ref(opt));
    pm.add(snir::PrettyPrinter{o1});

    auto nan = snir::Function{
        .type = snir::Type::Double,
        .name = "nan",
        .blocks =
            {
                snir::Block{
                    snir::NopInst{},
                    snir::NopInst{},
                },

                snir::Block{
                    snir::ConstInst{
                        .type = snir::Type::Int64,
                        .result = snir::Register{0},
                        .value = 1,
                    },
                    snir::ConstInst{
                        .type = snir::Type::Int64,
                        .result = snir::Register{1},
                        .value = 2,
                    },
                    snir::AddInst{
                        .type = snir::Type::Int64,
                        .result = snir::Register{2},
                        .lhs = snir::Register{0},
                        .rhs = snir::Register{1},
                    },
                },

                snir::Block{
                    snir::ConstInst{
                        .type = snir::Type::Double,
                        .result = snir::Register{3},
                        .value = 2.0,
                    },
                    snir::ConstInst{
                        .type = snir::Type::Double,
                        .result = snir::Register{4},
                        .value = 1.14159265359,
                    },
                    snir::FloatAddInst{
                        .type = snir::Type::Double,
                        .result = snir::Register{5},
                        .lhs = snir::Register{3},
                        .rhs = snir::Register{4},
                    },
                    snir::ReturnInst{
                        .type = snir::Type::Double,
                        .value = snir::Register{5},
                    },
                },
            },
    };

    pm(nan);

    return 0;
}
