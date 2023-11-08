#include "snir/core/file.hpp"
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

    static constexpr auto const logging = true;

    auto opt = snir::PassManager{logging};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});

    auto pm = snir::PassManager{logging};
    pm.add(std::ref(opt));
    pm.add(std::ref(opt));
    pm(nan);
    pm(sin);
    pm(ipow);
}

template<typename Inst>
[[nodiscard]] auto checkInstruction(char const* src) -> bool
{
    auto parser     = snir::Parser{};
    auto const inst = parser.parseInstruction(src);
    return inst.has_value() and inst->hasType<Inst>();
}

template<snir::Type Type>
[[nodiscard]] auto checkInstructionType(char const* src) -> bool
{
    auto parser     = snir::Parser{};
    auto const inst = parser.parseInstruction(src);
    return inst and inst->visit([](auto i) {
        if constexpr (requires { i.type; }) {
            return i.type == Type;
        } else {
            return false;
        }
    });
}

auto testParser() -> void
{
    auto parser = snir::Parser{};
    assert(parser.parseType("void") == snir::Type::Void);
    assert(parser.parseType("i1") == snir::Type::Bool);
    assert(parser.parseType("i64") == snir::Type::Int64);
    assert(parser.parseType("float") == snir::Type::Float);
    assert(parser.parseType("double") == snir::Type::Double);
    assert(parser.parseType("block") == snir::Type::Block);
    assert(parser.parseType("event") == snir::Type::Event);

    assert(checkInstruction<snir::ReturnInst>("ret %1"));
    assert(checkInstruction<snir::ConstInst>("%5 = i64 42"));
    assert(checkInstruction<snir::TruncInst>("%5 = trunc %1 as float"));
    assert(checkInstruction<snir::AddInst>("%5 = add i64 %3 %4"));
    assert(checkInstruction<snir::SubInst>("%5 = sub i64 %3 %4"));
    assert(checkInstruction<snir::MulInst>("%5 = mul i64 %3 %4"));
    assert(checkInstruction<snir::DivInst>("%5 = div i64 %3 %4"));
    assert(checkInstruction<snir::ModInst>("%5 = mod i64 %3 %4"));
    assert(checkInstruction<snir::AndInst>("%5 = and i64 %3 %4"));
    assert(checkInstruction<snir::OrInst>("%5 = or i64 %3 %4"));
    assert(checkInstruction<snir::XorInst>("%5 = xor i64 %3 %4"));
    assert(checkInstruction<snir::FloatAddInst>("%5 = fadd double %3 %4"));
    assert(checkInstruction<snir::FloatSubInst>("%5 = fsub double %3 %4"));
    assert(checkInstruction<snir::FloatMulInst>("%5 = fmul double %3 %4"));
    assert(checkInstruction<snir::FloatDivInst>("%5 = fdiv double %3 %4"));

    assert(checkInstructionType<snir::Type::Int64>("%5 = i64 42"));
    assert(checkInstructionType<snir::Type::Float>("%5 = float 42"));
    assert(checkInstructionType<snir::Type::Double>("%5 = double 42"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = add i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = sub i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = mul i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = div i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = mod i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = and i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = or i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Int64>("%5 = xor i64 %3 %4"));
    assert(checkInstructionType<snir::Type::Double>("%5 = fadd double %3 %4"));
    assert(checkInstructionType<snir::Type::Double>("%5 = fsub double %3 %4"));
    assert(checkInstructionType<snir::Type::Double>("%5 = fmul double %3 %4"));
    assert(checkInstructionType<snir::Type::Double>("%5 = fdiv double %3 %4"));

    {
        auto const* src = "%5 = add i64 %3 %4";
        auto const inst = parser.parseInstruction(src);
        assert(inst.has_value());
        assert(inst->hasType<snir::AddInst>());
        assert(inst->getResultRegister() == snir::Register{5});
    }

    {
        auto const* src = "%5 = i64 42";
        auto const inst = parser.parseInstruction(src);
        assert(inst.has_value());

        auto const operand = inst->getOperands().at(0).value();
        assert(std::holds_alternative<int>(operand));
        assert(std::get<int>(operand) == 42);
    }

    {
        auto const* src = "ret %5";
        auto const inst = parser.parseInstruction(src);
        assert(inst.has_value());
        assert(inst->getOperandRegisters().at(0).value() == snir::Register{5});
    }

    {
        auto const text   = snir::readFile("./test/files/func.ll").value();
        auto const module = parser.parseModule(text);
        assert(module.value().functions.size() == 1);

        auto const& func = module.value().functions[0];
        assert(func.name == "foo");
        assert(func.type == snir::Type::Double);
        assert(func.arguments.size() == 2);
        assert(func.blocks.size() == 1);

        auto const& block = func.blocks[0];
        assert(block.size() == 2);
        assert(block.at(0).hasType<snir::NopInst>());
        assert(block.at(1).hasType<snir::FloatAddInst>());
    }

    {
        auto const text   = snir::readFile("./test/files/funcs.ll").value();
        auto const module = parser.parseModule(text);
        assert(module.value().functions.size() == 3);

        assert(module->functions[0].name == "nan");
        assert(module->functions[0].type == snir::Type::Double);
        assert(module->functions[0].arguments.size() == 0);
        assert(module->functions[0].blocks.size() == 1);

        assert(module->functions[1].name == "sin");
        assert(module->functions[1].type == snir::Type::Float);
        assert(module->functions[1].arguments.size() == 1);
        assert(module->functions[1].arguments[0] == snir::Type::Float);
        assert(module->functions[1].blocks.size() == 1);

        assert(module->functions[2].name == "ipow");
        assert(module->functions[2].type == snir::Type::Int64);
        assert(module->functions[2].arguments.size() == 2);
        assert(module->functions[2].arguments[0] == snir::Type::Int64);
        assert(module->functions[2].arguments[1] == snir::Type::Int64);
        assert(module->functions[2].blocks.size() == 2);
    }
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
