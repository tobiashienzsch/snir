#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"
#include "snir/ir/instructions.hpp"
#include "snir/ir/parser.hpp"
#include "snir/ir/pass/dead_store_elimination.hpp"
#include "snir/ir/pass/remove_empty_block.hpp"
#include "snir/ir/pass/remove_nop.hpp"
#include "snir/ir/pass_manager.hpp"
#include "snir/ir/pretty_printer.hpp"
#include "snir/vm/interpreter.hpp"

#undef NDEBUG
#include <cassert>
#include <sstream>

namespace {

auto testPassManager() -> void
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
    auto sin = snir::Function{
        .type = snir::Type::Float,
        .name = "sin",
        .arguments = {snir::Type::Float},
        .blocks =
            {
                snir::Block{
                    snir::ConstInst{
                        .type = snir::Type::Double,
                        .result = snir::Register{1},
                        .value = 42,
                    },
                    snir::TruncInst{
                        .type = snir::Type::Float,
                        .result = snir::Register{2},
                        .value = snir::Register{1},
                    },
                    snir::ReturnInst{
                        .type = snir::Type::Float,
                        .value = snir::Register{2},
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
                        .result = snir::Register{2},
                        .value = 42,
                    },
                    snir::ReturnInst{
                        .type = snir::Type::Int64,
                        .value = snir::Register{2},
                    },
                },
            },
    };

    {
        auto out     = std::stringstream{};
        auto printer = snir::PrettyPrinter{out};
        printer(nan);
        printer(sin);
        printer(ipow);

        auto str = out.str();
        assert(str.find("define double @nan()") != std::string::npos);
        assert(str.find("define float @sin(float %0)") != std::string::npos);
        assert(str.find("define i64 @ipow(i64 %0, i64 %1)") != std::string::npos);
    }

    auto opt = snir::PassManager{true};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});

    auto pm = snir::PassManager{true};
    pm.add(std::ref(opt));
    pm.add(std::ref(opt));
    pm(nan);
    pm(sin);
    pm(ipow);
}

auto testParser() -> void
{

    auto const* const text = R"(
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
    // assert(module.value().functions.size() == 3);
}

auto testInterpreter() -> void
{
    using namespace snir;
    auto const r0  = Register{0};
    auto const r1  = Register{1};
    auto const r2  = Register{2};
    auto const v42 = Value{42};

    {
        // empty function
        auto vm = Interpreter{};

        auto const func   = Function{.type = Type::Void};
        auto const result = vm.execute(func, {});
        assert(result.has_value());
        assert(std::holds_alternative<std::nullopt_t>(result.value()));
    }

    {
        // function arg mismatch
        auto vm = Interpreter{};

        auto const func   = Function{.type = Type::Void, .arguments = std::vector{Type::Double}};
        auto const result = vm.execute(func, {});
        assert(not result.has_value());
    }

    {
        // return literal
        auto vm = Interpreter{};

        auto block  = snir::Block{Instruction{ReturnInst{.type = Type::Int64, .value = Value{42}}}};
        auto func   = Function{.type = Type::Int64, .blocks = {block}};
        auto result = vm.execute(func, {});
        assert(result.has_value());
        assert(std::holds_alternative<int>(result.value()));
        assert(std::get<int>(result.value()) == 42);
    }

    {
        // return constant<int>
        auto vm = Interpreter{};

        auto block = snir::Block{};
        block.push_back(ConstInst{.type = Type::Int64, .result = r0, .value = Value{143}});
        block.push_back(ReturnInst{.type = Type::Int64, .value = r0});
        auto func   = Function{.type = Type::Int64, .blocks = {block}};
        auto result = vm.execute(func, {});
        assert(result.has_value());
        assert(std::holds_alternative<int>(result.value()));
        assert(std::get<int>(result.value()) == 143);
    }

    {
        // return constant<float>
        auto vm = Interpreter{};

        auto block = snir::Block{};
        block.push_back(ConstInst{.type = Type::Float, .result = r0, .value = Value{42.0F}});
        block.push_back(ReturnInst{.type = Type::Float, .value = r0});
        auto func   = Function{.type = Type::Float, .blocks = {block}};
        auto result = vm.execute(func, {});
        assert(result.has_value());
        assert(std::holds_alternative<float>(result.value()));
        assert(std::get<float>(result.value()) == 42.0F);
    }

    {
        // return constant<double>
        auto vm = Interpreter{};

        auto block = snir::Block{};
        block.push_back(ConstInst{.type = Type::Double, .result = r0, .value = Value{42.0}});
        block.push_back(ReturnInst{.type = Type::Double, .value = r0});
        auto func   = Function{.type = Type::Double, .blocks = {block}};
        auto result = vm.execute(func, {});
        assert(result.has_value());
        assert(std::holds_alternative<double>(result.value()));
        assert(std::get<double>(result.value()) == 42.0);
    }

    {
        // return add<int>
        auto vm = Interpreter{};

        auto block = snir::Block{};
        block.push_back(ConstInst{.type = Type::Int64, .result = r0, .value = v42});
        block.push_back(AddInst{.type = Type::Int64, .result = r1, .lhs = r0, .rhs = v42});
        block.push_back(ReturnInst{.type = Type::Int64, .value = r1});
        auto func   = Function{.type = Type::Int64, .blocks = {block}};
        auto result = vm.execute(func, {});
        assert(result.has_value());
        assert(std::holds_alternative<int>(result.value()));
        assert(std::get<int>(result.value()) == 84);
    }

    {
        // return add<int>
        auto vm = Interpreter{};

        auto block = snir::Block{};
        block.push_back(ConstInst{.type = Type::Int64, .result = r0, .value = v42});
        block.push_back(SubInst{.type = Type::Int64, .result = r1, .lhs = r0, .rhs = v42});
        block.push_back(ReturnInst{.type = Type::Int64, .value = r1});
        auto func   = Function{.type = Type::Int64, .blocks = {block}};
        auto result = vm.execute(func, {});
        assert(result.has_value());
        assert(std::holds_alternative<int>(result.value()));
        assert(std::get<int>(result.value()) == 0);
    }
}

}  // namespace

auto main() -> int
{
    testPassManager();
    testParser();
    testInterpreter();
    return 0;
}
