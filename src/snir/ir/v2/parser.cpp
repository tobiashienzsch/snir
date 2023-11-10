#include "parser.hpp"

#include "snir/core/print.hpp"

#include <ctre.hpp>

namespace snir::v2 {

namespace {

[[nodiscard]] auto readType(std::string_view source) -> std::optional<Type>
{
#define SNIR_TYPE(Id, Name)                                                                          \
    if (source == std::string_view{#Name}) {                                                         \
        return Type::Id;                                                                             \
    }
#include "snir/ir/v2/type.def"
#undef SNIR_TYPE

    return std::nullopt;
}

[[nodiscard]] auto readFunctionArgs(std::string_view src) -> std::optional<std::vector<Type>>
{
    auto args = std::vector<Type>{};

    for (auto match : ctre::range<R"(\s*([a-zA-Z_]\w*)\s+%[0-9]+(?:\s*,\s*|$))">(src)) {
        auto const type = readType(match.get<1>());
        if (not type) {
            return std::nullopt;
        }

        args.push_back(type.value());
    }

    return args;
}

}  // namespace

Parser::Parser(Registry& registry) : _registry{&registry} {}

auto Parser::read(std::string_view source) -> std::optional<Module>
{
    auto module = Module{};

    for (auto match : ctre::range<R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})">(source)) {
        auto func  = _registry->create(ValueKind::Function);
        auto type  = func.emplace<Type>(readType(match.get<1>()).value());
        auto name  = func.emplace<Name>(match.get<2>().to_string());
        auto& args = func.emplace<FunctionArgs>(readFunctionArgs(match.get<3>()).value());

        println("define {} @{}({})", type, name.text, args.args.size());

        module.functions.push_back(func.getId());
    }

    return module;
}

}  // namespace snir::v2
