#include "snir/core/file.hpp"
#include "snir/core/strings.hpp"
#include "snir/ir/function.hpp"
#include "snir/ir/instruction.hpp"
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
    auto parser = snir::Parser{};

    for (auto const test : {
             std::string{"test/files/opt_dead_store.ll"},
             std::string{"test/files/opt_empty_block.ll"},
         }) {

        auto const src      = snir::readFile(test).value();
        auto const original = parser.parseModule(src).value();

        auto pm = snir::PassManager{true};
        pm.add(snir::DeadStoreElimination{});
        pm.add(snir::RemoveNop{});
        pm.add(snir::RemoveEmptyBlock{});

        auto optimized = original;
        auto& func     = optimized.functions.at(0);

        assert(func == original.functions.at(0));
        pm(func);
        assert(func != original.functions.at(0));
    }
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
    using namespace snir;

    auto parser = Parser{};
    assert(parser.parseType("void") == Type::Void);
    assert(parser.parseType("i1") == Type::Bool);
    assert(parser.parseType("i64") == Type::Int64);
    assert(parser.parseType("float") == Type::Float);
    assert(parser.parseType("double") == Type::Double);
    assert(parser.parseType("block") == Type::Block);
    assert(parser.parseType("event") == Type::Event);

    assert(checkInstruction<ReturnInst>("ret i64 %1"));
    assert(checkInstruction<ConstInst>("%5 = i64 42"));
    assert(checkInstruction<TruncInst>("%5 = trunc %1 as float"));
    assert(checkInstruction<AddInst>("%5 = add i64 %3 %4"));
    assert(checkInstruction<SubInst>("%5 = sub i64 %3 %4"));
    assert(checkInstruction<MulInst>("%5 = mul i64 %3 %4"));
    assert(checkInstruction<DivInst>("%5 = div i64 %3 %4"));
    assert(checkInstruction<ModInst>("%5 = mod i64 %3 %4"));
    assert(checkInstruction<AndInst>("%5 = and i64 %3 %4"));
    assert(checkInstruction<OrInst>("%5 = or i64 %3 %4"));
    assert(checkInstruction<XorInst>("%5 = xor i64 %3 %4"));
    assert(checkInstruction<FloatAddInst>("%5 = fadd double %3 %4"));
    assert(checkInstruction<FloatSubInst>("%5 = fsub double %3 %4"));
    assert(checkInstruction<FloatMulInst>("%5 = fmul double %3 %4"));
    assert(checkInstruction<FloatDivInst>("%5 = fdiv double %3 %4"));

    assert(checkInstructionType<Type::Int64>("ret i64 %1"));
    assert(checkInstructionType<Type::Int64>("%5 = icmp eq i64 %0 %1"));
    assert(checkInstructionType<Type::Int64>("%5 = i64 42"));
    assert(checkInstructionType<Type::Float>("%5 = float 42"));
    assert(checkInstructionType<Type::Double>("%5 = double 42"));
    assert(checkInstructionType<Type::Int64>("%5 = add i64 %3 %4"));
    assert(checkInstructionType<Type::Int64>("%5 = sub i64 %3 %4"));
    assert(checkInstructionType<Type::Int64>("%5 = mul i64 %3 %4"));
    assert(checkInstructionType<Type::Int64>("%5 = div i64 %3 %4"));
    assert(checkInstructionType<Type::Int64>("%5 = mod i64 %3 %4"));
    assert(checkInstructionType<Type::Int64>("%5 = and i64 %3 %4"));
    assert(checkInstructionType<Type::Int64>("%5 = or i64 %3 %4"));
    assert(checkInstructionType<Type::Int64>("%5 = xor i64 %3 %4"));
    assert(checkInstructionType<Type::Double>("%5 = fadd double %3 %4"));
    assert(checkInstructionType<Type::Double>("%5 = fsub double %3 %4"));
    assert(checkInstructionType<Type::Double>("%5 = fmul double %3 %4"));
    assert(checkInstructionType<Type::Double>("%5 = fdiv double %3 %4"));
    assert(checkInstructionType<Type::Float>("%5 = trunc %4 as float"));

    {
        auto const* src = "%5 = add i64 %3 %4";
        auto const inst = parser.parseInstruction(src);
        assert(inst.has_value());
        assert(inst->hasType<AddInst>());
        assert(inst->getResultRegister() == Register{5});

        auto const lhs = parser.parseInstruction(src);
        auto const rhs = parser.parseInstruction(src);
        assert(lhs == rhs);
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
        auto const* src = "ret i64 %5";
        auto const inst = parser.parseInstruction(src);
        assert(inst.has_value());
        assert(inst->get<ReturnInst>().type == Type::Int64);
        assert(std::get<Register>(inst->get<ReturnInst>().value) == Register{5});
    }

    {
        auto const text   = readFile("./test/files/func.ll").value();
        auto const module = parser.parseModule(text);
        assert(module.value().functions.size() == 1);

        auto const& func = module.value().functions[0];
        assert(func.name == "foo");
        assert(func.type == Type::Double);
        assert(func.arguments.size() == 2);
        assert(func.blocks.size() == 1);

        auto const& block = func.blocks[0];
        assert(block.size() == 2);
        assert(block.at(0).hasType<NopInst>());
        assert(block.at(1).hasType<FloatAddInst>());
    }

    {
        auto const text   = readFile("./test/files/funcs.ll").value();
        auto const module = parser.parseModule(text);
        assert(module.value().functions.size() == 3);

        auto const& funcs = module->functions;
        {
            auto const& nan = funcs[0];
            assert(nan.name == "nan");
            assert(nan.type == Type::Double);
            assert(nan.arguments.size() == 0);
            assert(nan.blocks.size() == 1);

            auto const& block = nan.blocks[0];
            assert(block.size() == 4);

            auto const i0 = block.at(0);
            assert(i0.hasType<ConstInst>());
            assert(i0.get<ConstInst>().type == Type::Double);
            assert(i0.get<ConstInst>().result == Register{3});
            assert(std::get<double>(i0.get<ConstInst>().value) == 2.0);

            auto const i1 = block.at(1);
            assert(i1.hasType<ConstInst>());
            assert(i1.get<ConstInst>().type == Type::Double);
            assert(i1.get<ConstInst>().result == Register{4});
            assert(std::get<double>(i1.get<ConstInst>().value) == 4.0);

            auto const i2 = block.at(2);
            assert(i2.hasType<FloatAddInst>());
            assert(i2.get<FloatAddInst>().type == Type::Double);
            assert(i2.get<FloatAddInst>().result == Register{5});
            assert(std::get<Register>(i2.get<FloatAddInst>().lhs) == Register{3});
            assert(std::get<Register>(i2.get<FloatAddInst>().rhs) == Register{4});

            auto const i3 = block.at(3);
            assert(i3.hasType<ReturnInst>());
            assert(std::get<Register>(i3.get<ReturnInst>().value) == Register{5});
        }

        {
            auto const& sin = funcs[1];
            assert(sin.name == "sin");
            assert(sin.type == Type::Float);
            assert(sin.arguments.size() == 1);
            assert(sin.arguments[0] == Type::Float);
            assert(sin.blocks.size() == 1);

            auto const& block = sin.blocks[0];
            assert(block.size() == 3);

            auto const i0 = block.at(0);
            assert(i0.hasType<ConstInst>());
            assert(i0.get<ConstInst>().result == Register{1});
            assert(std::get<double>(i0.get<ConstInst>().value) == 42.0);

            auto const i1 = block.at(1);
            assert(i1.hasType<TruncInst>());
            assert(i1.get<TruncInst>().type == Type::Float);
            assert(i1.get<TruncInst>().result == Register{2});
            assert(std::get<Register>(i1.get<TruncInst>().value) == Register{1});

            auto const i2 = block.at(2);
            assert(i2.hasType<ReturnInst>());
            assert(std::get<Register>(i2.get<ReturnInst>().value) == Register{2});
        }

        assert(funcs[2].name == "ipow");
        assert(funcs[2].type == Type::Int64);
        assert(funcs[2].arguments.size() == 2);
        assert(funcs[2].arguments[0] == Type::Int64);
        assert(funcs[2].arguments[1] == Type::Int64);
        assert(funcs[2].blocks.size() == 2);
    }
}

auto testInterpreter() -> void
{
    using namespace snir;

    {
        // empty function

        auto const func   = Function{.type = Type::Void};
        auto const result = Interpreter::execute(func, {});
        assert(not result.has_value());
    }

    {
        // function arg mismatch

        auto const func   = Function{.type = Type::Void, .arguments = std::vector{Type::Double}};
        auto const result = Interpreter::execute(func, {});
        assert(not result.has_value());
    }

    {
        // return files/i64_*.ll
        auto tests = std::vector<std::pair<std::string, int>>{};
        tests.emplace_back("./test/files/i64_add.ll", 42 + 143);
        tests.emplace_back("./test/files/i64_and.ll", 42 & 143);
        tests.emplace_back("./test/files/i64_blocks.ll", 9);
        tests.emplace_back("./test/files/i64_const.ll", 42);
        tests.emplace_back("./test/files/i64_icmp_eq_1.ll", 0);
        tests.emplace_back("./test/files/i64_icmp_eq_2.ll", 1);
        tests.emplace_back("./test/files/i64_icmp_ne_1.ll", 1);
        tests.emplace_back("./test/files/i64_icmp_ne_2.ll", 0);
        tests.emplace_back("./test/files/i64_div.ll", 42 / 2);
        tests.emplace_back("./test/files/i64_mul.ll", 42 * 143);
        tests.emplace_back("./test/files/i64_or.ll", 42 | 143);
        tests.emplace_back("./test/files/i64_mod.ll", 42 % 3);
        tests.emplace_back("./test/files/i64_shl.ll", 42 << 2);
        tests.emplace_back("./test/files/i64_shr.ll", 42 >> 2);
        tests.emplace_back("./test/files/i64_sub.ll", 42 - 143);
        tests.emplace_back("./test/files/i64_xor.ll", 42 ^ 143);

        for (auto const& [path, expected] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto parser = Parser{};
            auto module = parser.parseModule(src);
            assert(module.has_value());
            assert(module->functions.size() == 1);

            auto result = Interpreter::execute(module->functions.at(0), {});
            assert(result.has_value());
            assert(std::holds_alternative<int>(result.value()));
            assert(std::get<int>(result.value()) == expected);
        }
    }

    {
        // return files/float_*.ll
        auto tests = std::vector<std::pair<std::string, float>>{
            std::pair{"./test/files/float_add.ll",   42.0f + 143.0f},
            std::pair{"./test/files/float_div.ll",   42.0f / 2.0f  },
            std::pair{"./test/files/float_mul.ll",   42.0f * 143.0f},
            std::pair{"./test/files/float_sub.ll",   42.0f - 143.0f},
            std::pair{"./test/files/float_trunc.ll", 42.0f - 143.0f},
        };

        for (auto const& [path, expected] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto parser = Parser{};
            auto module = parser.parseModule(src);
            assert(module.has_value());
            assert(module->functions.size() == 1);

            auto result = Interpreter::execute(module->functions.at(0), {});
            assert(result.has_value());
            assert(std::holds_alternative<float>(result.value()));
            assert(std::get<float>(result.value()) == expected);
        }
    }

    {
        // return files/double_*.ll
        auto tests = std::vector<std::pair<std::string, double>>{
            std::pair{"./test/files/double_add.ll",   42.0 + 143.0},
            std::pair{"./test/files/double_div.ll",   42.0 / 2.0  },
            std::pair{"./test/files/double_mul.ll",   42.0 * 143.0},
            std::pair{"./test/files/double_sub.ll",   42.0 - 143.0},
            std::pair{"./test/files/double_trunc.ll", 42.0 - 143.0},
        };

        for (auto const& [path, expected] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto parser = Parser{};
            auto module = parser.parseModule(src);
            assert(module.has_value());
            assert(module->functions.size() == 1);

            auto result = Interpreter::execute(module->functions.at(0), {});
            assert(result.has_value());
            assert(std::holds_alternative<double>(result.value()));
            assert(std::get<double>(result.value()) == expected);
        }
    }
    {
        // return files/mismatch_float_*.ll
        auto const mismatch              = "type mismatch for instruction: 'f' vs 'd'";
        auto const unsupported_int_float = "unsupported type for int instruction: 'float'";
        auto const unsupported_float_i64 = "unsupported type for float instruction: 'i64'";

        auto const tests = std::vector<std::pair<std::string, std::string>>{
            std::make_pair("./test/files/mismatch_float_add_0.ll", mismatch),
            std::make_pair("./test/files/mismatch_float_add_1.ll", unsupported_int_float),
            std::make_pair("./test/files/mismatch_float_add_2.ll", unsupported_float_i64),
            std::make_pair("./test/files/mismatch_float_div.ll", mismatch),
            std::make_pair("./test/files/mismatch_float_mul.ll", mismatch),
            std::make_pair("./test/files/mismatch_float_sub.ll", mismatch),
        };

        for (auto const& [path, expected] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto parser = Parser{};
            auto module = parser.parseModule(src);
            assert(module.has_value());
            assert(module->functions.size() == 1);

            auto run = false;
            try {
                auto result = Interpreter::execute(module->functions.at(0), {});
                auto run    = true;
            } catch (std::exception const& e) {
                assert(strings::contains(e.what(), expected));
            }
            assert(not run);
        }
    }
}

auto testPrettyPrinter() -> void
{
    for (auto const entry : std::filesystem::directory_iterator{"./test/files"}) {
        snir::println("pretty-print/parse: {}", entry.path().string());

        auto src    = snir::readFile(entry.path()).value();
        auto parser = snir::Parser{};
        auto module = parser.parseModule(src);
        assert(module.has_value());

        auto stream  = std::stringstream{};
        auto printer = snir::PrettyPrinter{stream};
        printer(module.value());

        auto reconstructed = parser.parseModule(stream.str());
        if (module.value() != reconstructed.value()) {
            snir::println("'{}'", stream.str());
            assert(false);
        }
    }
}

}  // namespace

auto main() -> int
{
    testPassManager();
    testParser();
    testInterpreter();
    testPrettyPrinter();
    return 0;
}
