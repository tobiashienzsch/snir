#include "parser.hpp"

#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

#include <ctre.hpp>

#include <algorithm>
#include <array>

namespace snir::v2 {

namespace {
struct InstName
{
    InstKind kind;
    std::string_view name;
};

auto parseInstKind(std::string_view source) -> std::optional<InstKind>
{
    static constexpr auto names = std::array{
#define SNIR_INSTRUCTION(Id, Name) InstName{InstKind::Id, std::string_view{#Name}},
#include "snir/ir/v2/inst_kind.def"
#undef SNIR_INSTRUCTION
    };

    auto found = std::ranges::find(names, strings::trim(source), &InstName::name);
    if (found == std::ranges::end(names)) {
        return std::nullopt;
    }

    return found->kind;
}

[[nodiscard]] auto parseType(std::string_view source) -> std::optional<Type>
{
#define SNIR_TYPE(Id, Name)                                                                          \
    if (source == std::string_view{#Name}) {                                                         \
        return Type::Id;                                                                             \
    }
#include "snir/ir/v2/type.def"
#undef SNIR_TYPE

    return std::nullopt;
}

[[nodiscard]] auto parseArguments(std::string_view source) -> std::optional<std::vector<Type>>
{
    auto args = std::vector<Type>{};

    for (auto match : ctre::range<R"(\s*([a-zA-Z_]\w*)\s+%[0-9]+(?:\s*,\s*|$))">(source)) {
        auto const type = parseType(match.get<1>());
        if (not type) {
            return std::nullopt;
        }

        args.push_back(type.value());
    }

    return args;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto readBinaryInst(Registry& registry, std::string_view source) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(%(\d+) = (\w+) (\w+) (\d+|%\d+), (\d+|%\d+))">(source); match) {
        auto kind   = parseInstKind(match.get<2>().to_view()).value();
        auto type   = parseType(match.get<3>().to_view()).value();
        auto result = strings::parse<int>(match.get<1>());
        // auto lhs    = readValue(match.get<4>().to_view(), type).value();
        // auto rhs    = readValue(match.get<5>().to_view(), type).value();

        auto inst = registry.create(kind);
        inst.emplace<Type>(type);
        inst.emplace<Result>(Result{ValueId{result}});
        return inst;
    }

    return std::nullopt;
}

}  // namespace

Parser::Parser(Registry& registry) : _registry{&registry} {}

auto Parser::readModule(std::string_view source) -> std::optional<Module>
{
    auto module = Module{};

    for (auto match : ctre::range<R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})">(source)) {
        auto func  = _registry->create(ValueKind::Function);
        auto type  = func.emplace<Type>(parseType(match.get<1>()).value());
        auto name  = func.emplace<Name>(match.get<2>().to_string());
        auto& args = func.emplace<FunctionArguments>(parseArguments(match.get<3>()).value());
        auto& body = func.emplace<FunctionBody>(FunctionBody{});

        println("define {} @{}({}) {{ {} }}", type, name.text, args.args.size(), body.blocks.size());
        auto const blocks = readBasicBlocks(match.get<4>().to_string());
        if (not blocks) {
            return std::nullopt;
        }

        module.functions.push_back(func.getId());
    }

    return module;
}

auto Parser::readInst(std::string_view source) -> std::optional<Inst>
{
    if (auto inst = readBinaryInst(*_registry, source); inst) {
        return inst;
    }

    if (strings::contains(source, "; nop")) {
        return _registry->create(InstKind::Nop);
    }

    return std::nullopt;
}

auto Parser::readBasicBlocks(std::string_view source) -> std::optional<std::vector<BasicBlock>>
{
    auto blocks = std::vector<BasicBlock>{};

    for (auto match : ctre::split<R"(\d+:)">(source)) {
        auto const str = strings::trim(match, " \t\n");
        if (str.empty()) {
            continue;
        }

        auto block = readBasicBlock(str);
        if (not block) {
            return std::nullopt;
        }
        blocks.push_back(std::move(block.value()));
    }

    return blocks;
}

auto Parser::readBasicBlock(std::string_view source) -> std::optional<BasicBlock>
{
    auto block = BasicBlock{};

    for (auto match : ctre::split<R"(\n)">(source)) {
        auto inst = readInst(strings::trim(match, " \t"));
        if (inst) {
            block.instructions.push_back(inst.value().getId());
        }
    }

    return block;
}

}  // namespace snir::v2
