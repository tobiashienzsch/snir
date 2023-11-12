#include "snir/ir/v3/CompareKind.hpp"
#include "snir/ir/v3/Identifier.hpp"
#include "snir/ir/v3/InstKind.hpp"
#include "snir/ir/v3/Interpreter.hpp"
#include "snir/ir/v3/Parser.hpp"
#include "snir/ir/v3/Printer.hpp"
#include "snir/ir/v3/Registry.hpp"
#include "snir/ir/v3/Type.hpp"

#include "snir/core/file.hpp"
#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

#include <ctre.hpp>

#undef NDEBUG
#include <cassert>
#include <filesystem>

namespace ir = snir::v3;

struct FunctionTestSpec
{
    std::string name{};
    std::string type{};
    std::size_t args{std::numeric_limits<std::size_t>::max()};
    std::size_t blocks{std::numeric_limits<std::size_t>::max()};
    std::size_t instructions{std::numeric_limits<std::size_t>::max()};
};

[[nodiscard]] auto getBetween(std::string_view s, std::string_view start, std::string_view stop)
    -> std::string_view
{
    auto const startPos   = s.find(start);
    auto const endOfStart = startPos + start.length();
    auto const stopPos    = s.find(stop);
    return s.substr(endOfStart, stopPos - endOfStart);
}

auto forEachLine(std::string_view str, auto callback) -> void
{
    if (str.empty()) {
        return;
    }

    auto first = 0;
    while (true) {
        auto const last = str.find_first_of('\n', first);
        if (last == std::string_view::npos) {
            return;
        }

        auto const line = str.substr(first, last - first);
        callback(line);
        first = last + 1;
    }
}

[[nodiscard]] auto parseFunctionTestSpec(std::string_view source) -> FunctionTestSpec
{
    auto spec = getBetween(source, "; BEGIN_TEST", "; END_TEST");
    auto test = FunctionTestSpec{};
    forEachLine(spec, [&](std::string_view line) {
        auto trimmed = snir::strings::trim(line);
        if (trimmed.empty()) {
            return;
        }
        if (auto match = ctre::match<R"(;\s+name:\s+(\w+))">(line); match) {
            test.name = match.get<1>();
            return;
        }
        if (auto match = ctre::match<R"(;\s+type:\s+(\w+))">(line); match) {
            test.type = match.get<1>();
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

        snir::raisef<std::invalid_argument>("could not parse test spec '{}'", line);
    });

    return test;
}

auto main() -> int
{
    for (auto const& entry : std::filesystem::directory_iterator{"./test/v3"}) {
        auto registry = ir::Registry{};
        auto parser   = ir::Parser{registry};
        auto source   = snir::readFile(entry).value();
        auto test     = parseFunctionTestSpec(source);
        auto module   = parser.read(source);
        assert(module.has_value());
        assert(module->getFunctions().size() == 1);

        auto const funcId   = module->getFunctions().at(0);
        auto const funcView = registry.view<ir::Type, ir::Identifier, ir::FunctionDefinition>();
        auto const [type, name, func] = funcView.get(funcId);
        assert(std::format("{}", type) == test.type);
        assert(name.text == test.name);
        assert(func.args.size() == test.args);
        assert(func.blocks.size() == test.blocks);

        auto instCount = 0U;
        for (auto const& block : func.blocks) {
            for ([[maybe_unused]] auto const inst : block.instructions) {
                ++instCount;
            }
        }
        assert(instCount == test.instructions);

        auto result = std::optional<ir::Literal>{};
        if (func.args.empty()) {
            auto vm = ir::Interpreter{registry};
            result  = vm.execute(func, {});
            assert(result.has_value());

            if (type == ir::Type::Void) {
                assert(std::isnan(std::get<double>(result->value)));
            } else if (type == ir::Type::Int64) {
                assert(std::holds_alternative<std::int64_t>(result->value));
            } else if (type == ir::Type::Float) {
                assert(std::holds_alternative<float>(result->value));
            } else if (type == ir::Type::Double) {
                assert(std::holds_alternative<double>(result->value));
            } else {
                snir::raisef<std::runtime_error>("unimplemented return type '{}'", type);
            }
        }

        snir::println("; {}", entry.path().string());
        if (result) {
            snir::println("; return: {} as {}", *result, type);
        }
        auto printer = ir::Printer{std::cout};
        printer(*module);
    }

    return 0;
}
