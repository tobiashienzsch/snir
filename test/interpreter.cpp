#include "snir/ir/Interpreter.hpp"
#include "snir/ir/CompareKind.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Identifier.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Literal.hpp"
#include "snir/ir/Parser.hpp"
#include "snir/ir/Registry.hpp"
#include "snir/ir/Type.hpp"

#include "snir/core/file.hpp"
#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

#include <ctre.hpp>

#undef NDEBUG
#include <cassert>
#include <filesystem>

struct FunctionTestSpec
{
    std::string name{};
    snir::Type type{};
    std::size_t args{std::numeric_limits<std::size_t>::max()};
    std::size_t blocks{std::numeric_limits<std::size_t>::max()};
    std::size_t instructions{std::numeric_limits<std::size_t>::max()};
    snir::Literal result;
};

[[nodiscard]] auto getBetween(std::string_view s, std::string_view start, std::string_view stop)
    -> std::string_view
{
    auto const startPos   = s.find(start);
    auto const endOfStart = startPos + start.length();
    auto const stopPos    = s.find(stop);
    return s.substr(endOfStart, stopPos - endOfStart);
}

[[nodiscard]] auto parseFunctionTestSpec(std::string_view source) -> FunctionTestSpec
{
    auto spec = getBetween(source, "; BEGIN_TEST", "; END_TEST");
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
            if (test.type == snir::Type::Void) {
                assert(literal == "void");
                test.result = snir::Literal{std::nan("")};
                return;
            }
            if (test.type == snir::Type::Bool) {
                test.result = snir::Literal{literal == "true"};
                return;
            }
            if (test.type == snir::Type::Int64) {
                test.result = snir::Literal{snir::strings::parse<std::int64_t>(literal)};
                return;
            }
            if (test.type == snir::Type::Float) {
                test.result = snir::Literal{snir::strings::parse<float>(literal)};
                return;
            }
            if (test.type == snir::Type::Double) {
                test.result = snir::Literal{snir::strings::parse<double>(literal)};
                return;
            }
            snir::raisef<std::invalid_argument>("unknown literal '{}'", literal);
        }

        snir::raisef<std::invalid_argument>("could not parse test spec '{}'", line);
    });

    return test;
}

auto main() -> int
{
    for (auto const& entry : std::filesystem::directory_iterator{"./test/files"}) {
        snir::println("; {}", entry.path().string());

        auto registry = snir::Registry{};
        auto parser   = snir::Parser{registry};
        auto source   = snir::readFile(entry).value();
        auto test     = parseFunctionTestSpec(source);
        auto module   = parser.read(source);
        assert(module.has_value());
        assert(module->getFunctions().size() == 1);

        auto const func = snir::Function{registry, module->getFunctions().at(0)};
        assert(func.getType() == test.type);
        assert(func.getIdentifier() == test.name);
        assert(func.getArguments().size() == test.args);
        assert(func.getBasicBlocks().size() == test.blocks);
        assert(func.getInstructionCount() == test.instructions);

        if (func.getArguments().empty()) {
            auto vm     = snir::Interpreter{};
            auto result = vm.execute(func, {});
            assert(result.has_value());

            snir::println("; return: {} as {}", *result, func.getType());
            if (func.getType() == snir::Type::Void) {
                assert(std::isnan(std::get<double>(result->value)));
            } else {
                assert(result->value == test.result.value);
            }
        }
    }

    return 0;
}
