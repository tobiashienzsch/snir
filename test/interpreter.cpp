#undef NDEBUG
#include "snir/ir/Interpreter.hpp"
#include "snir/core/File.hpp"
#include "snir/core/Strings.hpp"
#include "snir/ir/CompareKind.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Identifier.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Literal.hpp"
#include "snir/ir/Parser.hpp"
#include "snir/ir/pass/DeadStoreElimination.hpp"
#include "snir/ir/pass/RemoveEmptyBlock.hpp"
#include "snir/ir/pass/RemoveNop.hpp"
#include "snir/ir/PassManager.hpp"
#include "snir/ir/Printer.hpp"
#include "snir/ir/Registry.hpp"
#include "snir/ir/Type.hpp"

#include <ctre.hpp>

#include <cassert>
#include <filesystem>
#include <print>

namespace {

struct FunctionTestSpec
{
    std::string name;
    snir::Type type{};
    std::size_t args{std::numeric_limits<std::size_t>::max()};
    std::size_t blocks{std::numeric_limits<std::size_t>::max()};
    std::size_t instructions{std::numeric_limits<std::size_t>::max()};
    snir::Literal result;
};

[[nodiscard]] auto parseTestResult(std::string_view str, snir::Type type) -> snir::Literal
{
    if (type == snir::Type::Void) {
        assert(str == "void");
        return snir::Literal{std::nan("")};
    }
    if (type == snir::Type::Bool) {
        return snir::Literal{str == "true"};
    }
    if (type == snir::Type::Int64) {
        return snir::Literal{snir::strings::parse<std::int64_t>(str)};
    }
    if (type == snir::Type::Float) {
        return snir::Literal{snir::strings::parse<float>(str)};
    }
    if (type == snir::Type::Double) {
        return snir::Literal{snir::strings::parse<double>(str)};
    }

    snir::raisef<std::invalid_argument>("unknown literal '{}'", str);
}

[[nodiscard]] auto parseFunctionTestSpec(std::string_view source) -> FunctionTestSpec
{
    auto spec = snir::strings::getBetween(source, "; BEGIN_TEST", "; END_TEST");
    auto test = FunctionTestSpec{};
    snir::strings::forEachLine(spec, [&](std::string_view line) {
        auto trimmed = snir::strings::trim(line);
        if (trimmed.empty()) {
            return;
        }
        if (auto match = ctre::match<R"(;\s+name:\s+(\w+))">(line); match) {
            test.name = match.get<1>();
            return;
        }
        if (auto match = ctre::match<R"(;\s+type:\s+(\w+))">(line); match) {
            test.type = snir::parseType(match.get<1>());
            return;
        }
        if (auto match = ctre::match<R"(;\s+args:\s+(\d+))">(line); match) {
            test.args = static_cast<std::size_t>(match.get<1>().to_number());
            return;
        }
        if (auto match = ctre::match<R"(;\s+blocks:\s+(\d+))">(line); match) {
            test.blocks = static_cast<std::size_t>(match.get<1>().to_number());
            return;
        }
        if (auto match = ctre::match<R"(;\s+instructions:\s+(\d+))">(line); match) {
            test.instructions = static_cast<std::size_t>(match.get<1>().to_number());
            return;
        }
        if (auto match = ctre::match<R"(;\s+return:\s+(\S+))">(line); match) {
            auto const literal = snir::strings::trim(match.get<1>());
            test.result        = parseTestResult(literal, test.type);
            return;
        }

        snir::raisef<std::invalid_argument>("could not parse test spec '{}'", line);
    });

    return test;
}

auto execute(snir::Function const& func, snir::Literal expected) -> void
{
    if (func.arguments().empty()) {
        auto vm     = snir::Interpreter{};
        auto result = vm.execute(func, {});
        assert(result.has_value());

        std::println("; return: {} as {}", *result, func.type());
        if (func.type() == snir::Type::Void) {
            assert(std::isnan(std::get<double>(result->value)));
        } else {
            assert(result->value == expected.value);
        }
    }
}

auto optimize(snir::Module& module) -> void
{
    auto opt = snir::PassManager{true};
    opt.add(snir::DeadStoreElimination{});
    opt.add(snir::RemoveNop{});
    opt.add(snir::RemoveEmptyBlock{});

    auto pm      = snir::PassManager{true};
    auto printer = snir::Printer{std::cout};
    pm.add(std::ref(printer));
    pm.add(std::ref(opt));
    pm.add(std::ref(opt));
    pm.add(std::ref(printer));
    pm(module);
}

auto testFile(std::filesystem::path const& path) -> void
{
    std::println("; {}", path.string());

    auto registry = snir::Registry{};
    auto parser   = snir::Parser{registry};
    auto source   = snir::readFile(path).value();
    auto test     = parseFunctionTestSpec(source);
    auto module   = parser.read(source);
    assert(module.functions().size() == 1);

    auto const func = snir::Function{registry, module.functions().at(0)};
    assert(func.type() == test.type);
    assert(func.identifier() == test.name);
    assert(func.arguments().size() == test.args);
    assert(func.basicBlocks().size() == test.blocks);
    assert(func.numInstructions() == test.instructions);

    execute(func, test.result);
    optimize(module);
    execute(func, test.result);
}

}  // namespace

auto main() -> int
{
    for (auto const& entry : std::filesystem::directory_iterator{"./test/files"}) {
        if (not entry.is_regular_file()) {
            continue;
        }
        testFile(entry);
    }

    return EXIT_SUCCESS;
}
