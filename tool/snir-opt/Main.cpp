#include "snir/Function.hpp"
#include "snir/Instruction.hpp"
#include "snir/Instructions.hpp"
#include "snir/OptimisationPass.hpp"
#include "snir/Parser.hpp"
#include "snir/PassManager.hpp"
#include "snir/PrettyPrinter.hpp"

auto main() -> int
{
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
                        .destination = snir::Register{0},
                        .literal = snir::Literal{1},
                    },
                    snir::ConstInst{
                        .type = snir::Type::Int64,
                        .destination = snir::Register{1},
                        .literal = snir::Literal{2},
                    },
                    snir::AddInst{
                        .type = snir::Type::Int64,
                        .destination = snir::Register{2},
                        .lhs = snir::Register{0},
                        .rhs = snir::Register{1},
                    },
                },

                snir::Block{
                    snir::ConstInst{
                        .type = snir::Type::Double,
                        .destination = snir::Register{3},
                        .literal = snir::Literal{2.0},
                    },
                    snir::ConstInst{
                        .type = snir::Type::Double,
                        .destination = snir::Register{4},
                        .literal = snir::Literal{1.14159265359},
                    },
                    snir::FloatAddInst{
                        .type = snir::Type::Double,
                        .destination = snir::Register{5},
                        .lhs = snir::Register{3},
                        .rhs = snir::Register{4},
                    },
                    snir::ReturnInst{
                        .type = snir::Type::Double,
                        .operand = snir::Register{5},
                    },
                },
            },
    };
    auto sin = snir::Function{
        .type = snir::Type::Float,
        .name = "sin",
        .arguments = {snir::Type::Float},
        .blocks =
            {
                snir::Block{
                    snir::ConstInst{
                        .type = snir::Type::Double,
                        .destination = snir::Register{1},
                        .literal = snir::Literal{42},
                    },
                    snir::TruncInst{
                        .type = snir::Type::Float,
                        .destination = snir::Register{2},
                        .operand = snir::Register{1},
                    },
                    snir::ReturnInst{
                        .type = snir::Type::Float,
                        .operand = snir::Register{2},
                    },
                },
            },
    };
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
                        .literal = snir::Literal{42},
                    },
                    snir::ReturnInst{
                        .type = snir::Type::Int64,
                        .operand = snir::Register{2},
                    },
                },
            },
    };

    auto o0 = std::fopen("snir_O0.ll", "w");
    auto o1 = std::fopen("snir_O1.ll", "w");

    auto opt = snir::PassManager{true};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});

    auto pm = snir::PassManager{true};
    pm.add(snir::PrettyPrinter{o0});
    pm.add(std::ref(opt));
    pm.add(std::ref(opt));
    pm.add(snir::PrettyPrinter{o1});

    pm(nan);
    pm(sin);
    pm(ipow);

    std::fclose(o0);
    std::fclose(o1);

    auto const text = R"(
define double @nan() {
0:
  %3 = double 2
  %4 = double 1.14159265359
  %5 = fadd double %3 %4
  ret %5
}

define float @sin(float %0) {
0:
  %1 = double 42
  %2 = trunc %1 as float
  ret %2
}

define i64 @ipow(i64 %0, i64 %1) {
0:
  %2 = i64 42
  ret %2
}
)";

    auto parser       = snir::Parser{};
    auto const module = parser(text);

    return 0;
}
