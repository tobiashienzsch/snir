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
#include <map>
#include <sstream>
#include <utility>

namespace {

auto testVector() -> void  // NOLINT(readability-function-cognitive-complexity)
{
    auto test = []<typename T>(T val) {  // NOLINT(readability-function-cognitive-complexity)
        using Vec = snir::StaticVector<T, 2>;
        static_assert(std::same_as<typename Vec::value_type, T>);
        static_assert(std::same_as<typename Vec::value_type, T>);

        auto vec = Vec{};
        assert(vec.empty());
        assert(not vec.full());
        assert(vec.capacity() == 2);
        assert(vec.size() == 0);  // NOLINT

        vec.push_back(val);
        assert(not vec.empty());
        assert(not vec.full());
        assert(vec.size() == 1);
        assert(std::distance(vec.begin(), vec.end()) == 1);
        assert(std::distance(std::ranges::begin(vec), std::ranges::end(vec)) == 1);
        assert(std::distance(std::as_const(vec).begin(), std::as_const(vec).end()) == 1);
        assert(vec[0] == val);

        vec.push_back(static_cast<T>(val + val));
        assert(not vec.empty());
        assert(vec.size() == 2);
        assert(vec.full());
        assert(std::distance(vec.begin(), vec.end()) == 2);
        assert(std::distance(std::ranges::begin(vec), std::ranges::end(vec)) == 2);
        assert(std::distance(std::as_const(vec).begin(), std::as_const(vec).end()) == 2);
        assert(vec[0] == val);
        assert(vec[1] == val + val);
        assert(std::as_const(vec)[0] == val);
        assert(std::as_const(vec)[1] == val + val);

        auto const other = vec;
        assert(std::ranges::equal(vec, other));

        auto const list = Vec{val, val};
        assert(list.size() == 2);
        assert(list[0] == val);
        assert(list[1] == val);

        try {
            [[maybe_unused]] auto tooBig = Vec{val, val, val};
            assert(false);
        } catch (const std::exception& e) {
            assert(snir::strings::contains(e.what(), "initializer_list out-of-bounds size: 3"));
        }

        try {
            [[maybe_unused]] auto newVal = vec[42];
            assert(false);
        } catch (const std::exception& e) {
            assert(snir::strings::contains(e.what(), "subscript out-of-bounds idx: 42, size: 2"));
        }

        try {
            [[maybe_unused]] auto newVal = vec.push_back(val);
            assert(false);
        } catch (const std::exception& e) {
            assert(snir::strings::contains(e.what(), "push_back on full StaticVector<T, 2>"));
        }
    };

    test(std::uint8_t{42});
    test(std::uint16_t{42});
    test(std::uint32_t{42});
    test(std::uint64_t{42});
    test(std::int8_t{42});
    test(std::int16_t{42});
    test(std::int32_t{42});
    test(std::int64_t{42});
    test(float{42});
    test(double{42});

    static_assert(sizeof(snir::StaticVector<std::uint8_t, 1>{}) == 2);
    static_assert(sizeof(snir::StaticVector<std::uint8_t, 2>{}) == 3);
    static_assert(sizeof(snir::StaticVector<std::uint8_t, 3>{}) == 4);

    static_assert(sizeof(snir::StaticVector<std::uint16_t, 1>{}) == 4);
    static_assert(sizeof(snir::StaticVector<std::uint16_t, 2>{}) == 6);
    static_assert(sizeof(snir::StaticVector<std::uint16_t, 3>{}) == 8);

    static_assert(sizeof(snir::StaticVector<std::uint32_t, 1>{}) == 8);
    static_assert(sizeof(snir::StaticVector<std::uint32_t, 2>{}) == 12);
    static_assert(sizeof(snir::StaticVector<std::uint32_t, 3>{}) == 16);
}

auto testPassManager() -> void
{
    for (auto const& test : {
             std::string{"./test/files/opt_dead_store.ll"},
             std::string{"./test/files/opt_empty_block.ll"},
         }) {

        auto const src      = snir::readFile(test).value();
        auto const original = snir::Parser::readModule(src).value();

        auto pm = snir::PassManager{true};
        pm.add(snir::DeadStoreElimination{});
        pm.add(snir::RemoveNop{});
        pm.add(snir::RemoveEmptyBasicBlock{});

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
    auto const inst = snir::Parser::readInstruction(src);
    return inst.has_value() and inst->hasType<Inst>();
}

template<snir::Type Type>
[[nodiscard]] auto checkInstructionType(char const* src) -> bool
{
    auto const inst = snir::Parser::readInstruction(src);
    return inst and inst->visit([](auto i) {
        if constexpr (requires { i.type; }) {
            return i.type == Type;
        } else {
            return false;
        }
    });
}

auto testParser() -> void  // NOLINT(readability-function-cognitive-complexity)
{
    using namespace snir;

    assert(snir::Parser::readType("void") == Type::Void);
    assert(snir::Parser::readType("i1") == Type::Bool);
    assert(snir::Parser::readType("i64") == Type::Int64);
    assert(snir::Parser::readType("float") == Type::Float);
    assert(snir::Parser::readType("double") == Type::Double);
    assert(snir::Parser::readType("block") == Type::Block);
    assert(snir::Parser::readType("event") == Type::Event);

    assert(checkInstruction<ReturnInst>("ret i64 %1"));
    assert(checkInstruction<BranchInst>("br label %1"));
    assert(checkInstruction<BranchInst>("br i1 %0, label %1, label %2"));
    assert(checkInstruction<ConstInst>("%5 = i64 42"));
    assert(checkInstruction<TruncInst>("%5 = trunc %1 to float"));
    assert(checkInstruction<AddInst>("%5 = add i64 %3, %4"));
    assert(checkInstruction<SubInst>("%5 = sub i64 %3, %4"));
    assert(checkInstruction<MulInst>("%5 = mul i64 %3, %4"));
    assert(checkInstruction<DivInst>("%5 = div i64 %3, %4"));
    assert(checkInstruction<ModInst>("%5 = mod i64 %3, %4"));
    assert(checkInstruction<AndInst>("%5 = and i64 %3, %4"));
    assert(checkInstruction<OrInst>("%5 = or i64 %3, %4"));
    assert(checkInstruction<XorInst>("%5 = xor i64 %3, %4"));
    assert(checkInstruction<FloatAddInst>("%5 = fadd double %3, %4"));
    assert(checkInstruction<FloatSubInst>("%5 = fsub double %3, %4"));
    assert(checkInstruction<FloatMulInst>("%5 = fmul double %3, %4"));
    assert(checkInstruction<FloatDivInst>("%5 = fdiv double %3, %4"));

    assert(checkInstructionType<Type::Int64>("ret i64 %1"));
    assert(checkInstructionType<Type::Int64>("%5 = icmp eq i64 %0, %1"));
    assert(checkInstructionType<Type::Int64>("%5 = i64 42"));
    assert(checkInstructionType<Type::Float>("%5 = float 42"));
    assert(checkInstructionType<Type::Double>("%5 = double 42"));
    assert(checkInstructionType<Type::Int64>("%5 = add i64 %3, %4"));
    assert(checkInstructionType<Type::Int64>("%5 = sub i64 %3, %4"));
    assert(checkInstructionType<Type::Int64>("%5 = mul i64 %3, %4"));
    assert(checkInstructionType<Type::Int64>("%5 = div i64 %3, %4"));
    assert(checkInstructionType<Type::Int64>("%5 = mod i64 %3, %4"));
    assert(checkInstructionType<Type::Int64>("%5 = and i64 %3, %4"));
    assert(checkInstructionType<Type::Int64>("%5 = or i64 %3, %4"));
    assert(checkInstructionType<Type::Int64>("%5 = xor i64 %3, %4"));
    assert(checkInstructionType<Type::Double>("%5 = fadd double %3, %4"));
    assert(checkInstructionType<Type::Double>("%5 = fsub double %3, %4"));
    assert(checkInstructionType<Type::Double>("%5 = fmul double %3, %4"));
    assert(checkInstructionType<Type::Double>("%5 = fdiv double %3, %4"));
    assert(checkInstructionType<Type::Float>("%5 = trunc %4 to float"));

    {
        auto const* src = "%5 = add i64 %3, %4";
        auto const inst = snir::Parser::readInstruction(src);
        assert(inst.has_value());
        assert(inst->hasType<AddInst>());
        assert(inst->getResultRegister() == Register{5});

        auto const lhs = snir::Parser::readInstruction(src);
        auto const rhs = snir::Parser::readInstruction(src);
        assert(lhs == rhs);
    }

    {
        auto const* src = "%5 = i64 42";
        auto const inst = snir::Parser::readInstruction(src);
        assert(inst.has_value());

        auto const operand = inst->getOperands()[0];
        assert(std::holds_alternative<int>(operand));
        assert(std::get<int>(operand) == 42);
    }

    {
        auto const* src = "ret i64 %5";
        auto const inst = snir::Parser::readInstruction(src);
        assert(inst.has_value());
        assert(inst->get<ReturnInst>().type == Type::Int64);
        assert(std::get<Register>(inst->get<ReturnInst>().value) == Register{5});
    }

    {
        auto const text   = readFile("./test/files/func.ll").value();
        auto const module = snir::Parser::readModule(text);
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
        auto const module = snir::Parser::readModule(text);
        assert(module.value().functions.size() == 3);

        auto const& funcs = module->functions;
        {
            auto const& nan = funcs[0];
            assert(nan.name == "nan");
            assert(nan.type == Type::Double);
            assert(nan.arguments.empty());
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

auto testInterpreter() -> void  // NOLINT(readability-function-cognitive-complexity)
{
    using namespace snir;

    {
        // empty function
        auto const func   = Function{.type = Type::Void, .name = {}, .arguments = {}, .blocks = {}};
        auto const result = Interpreter::execute(func, {});
        assert(not result.has_value());
    }

    {
        // function arg mismatch
        auto const func = Function{
            .type      = Type::Void,
            .name      = {},
            .arguments = std::vector{Type::Double},
            .blocks    = {},
        };

        auto const result = Interpreter::execute(func, {});
        assert(not result.has_value());
    }

    {
        // return files/i64_*.ll
        auto tests = std::vector<std::tuple<std::string, int, std::vector<Value>>>{};
        tests.emplace_back("./test/files/i64_add.ll", 42 + 143, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_and.ll", int(42U & 143U), std::vector<Value>{});
        tests.emplace_back("./test/files/i64_args_1.ll", 42, std::vector<Value>{42});
        tests.emplace_back("./test/files/i64_args_1.ll", 143, std::vector<Value>{143});
        tests.emplace_back("./test/files/i64_args_2.ll", 42 + 2, std::vector<Value>{42, 2});
        tests.emplace_back("./test/files/i64_args_2.ll", 143 + 2, std::vector<Value>{143, 2});
        tests.emplace_back("./test/files/i64_blocks.ll", 9, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_branch.ll", 9, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_const.ll", 42, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_icmp_eq_1.ll", 0, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_icmp_eq_2.ll", 1, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_icmp_ne_1.ll", 1, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_icmp_ne_2.ll", 0, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_div.ll", 42 / 2, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_mul.ll", 42 * 143, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_or.ll", int(42U | 143U), std::vector<Value>{});
        tests.emplace_back("./test/files/i64_mod.ll", 42 % 3, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_shl.ll", int(42U << 2U), std::vector<Value>{});
        tests.emplace_back("./test/files/i64_shr.ll", int(42U >> 2U), std::vector<Value>{});
        tests.emplace_back("./test/files/i64_sub.ll", 42 - 143, std::vector<Value>{});
        tests.emplace_back("./test/files/i64_xor.ll", int(42U ^ 143U), std::vector<Value>{});

        for (auto const& [path, expected, args] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto module = snir::Parser::readModule(src);
            assert(module.has_value());
            assert(module->functions.size() == 1);

            auto result = Interpreter::execute(module->functions.at(0), args);
            assert(result.has_value());
            assert(std::holds_alternative<int>(result.value()));
            assert(std::get<int>(result.value()) == expected);
        }
    }

    {
        // return files/float_*.ll
        auto tests = std::vector<std::pair<std::string, float>>{};
        tests.emplace_back("./test/files/float_add.ll", 42.0F + 143.0F);
        tests.emplace_back("./test/files/float_div.ll", 42.0F / 2.0F);
        tests.emplace_back("./test/files/float_mul.ll", 42.0F * 143.0F);
        tests.emplace_back("./test/files/float_sub.ll", 42.0F - 143.0F);
        tests.emplace_back("./test/files/float_trunc.ll", 42.0F - 143.0F);

        for (auto const& [path, expected] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto module = snir::Parser::readModule(src);
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
        auto tests = std::vector<std::pair<std::string, double>>{};
        tests.emplace_back("./test/files/double_add.ll", 42.0 + 143.0);
        tests.emplace_back("./test/files/double_div.ll", 42.0 / 2.0);
        tests.emplace_back("./test/files/double_mul.ll", 42.0 * 143.0);
        tests.emplace_back("./test/files/double_sub.ll", 42.0 - 143.0);
        tests.emplace_back("./test/files/double_trunc.ll", 42.0 - 143.0);

        for (auto const& [path, expected] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto module = snir::Parser::readModule(src);
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
        auto const* const mismatch            = "type mismatch for instruction: 'f' vs 'd'";
        auto const* const unsupportedIntFloat = "unsupported type for int instruction: 'float'";
        auto const* const unsupportedFloatI64 = "unsupported type for float instruction: 'i64'";

        auto tests = std::vector<std::pair<std::string, std::string>>{};
        tests.emplace_back("./test/files/mismatch_float_add_0.ll", mismatch);
        tests.emplace_back("./test/files/mismatch_float_add_1.ll", unsupportedIntFloat);
        tests.emplace_back("./test/files/mismatch_float_add_2.ll", unsupportedFloatI64);
        tests.emplace_back("./test/files/mismatch_float_div.ll", mismatch);
        tests.emplace_back("./test/files/mismatch_float_mul.ll", mismatch);
        tests.emplace_back("./test/files/mismatch_float_sub.ll", mismatch);

        for (auto const& [path, expected] : tests) {
            println("execute: {}", path);

            auto src    = readFile(path).value();
            auto module = snir::Parser::readModule(src);
            assert(module.has_value());
            assert(module->functions.size() == 1);

            try {
                [[maybe_unused]] auto result = Interpreter::execute(module->functions.at(0), {});
                assert(false);
            } catch (std::exception const& e) {
                assert(strings::contains(e.what(), expected));
            }
        }
    }
}

auto testPrettyPrinter() -> void
{
    for (auto const& entry : std::filesystem::directory_iterator{"./test/files"}) {
        snir::println("pretty-print/parse: {}", entry.path().string());

        auto src    = snir::readFile(entry.path()).value();
        auto module = snir::Parser::readModule(src);
        assert(module.has_value());

        auto stream  = std::stringstream{};
        auto printer = snir::PrettyPrinter{stream};
        printer(module.value());

        auto reconstructed = snir::Parser::readModule(stream.str());
        if (module.value() != reconstructed.value()) {
            snir::println("'{}'", stream.str());
            assert(false);
        }
    }
}

}  // namespace

auto main() -> int
{
    testVector();
    testPassManager();
    testParser();
    testInterpreter();
    testPrettyPrinter();
    return 0;
}
