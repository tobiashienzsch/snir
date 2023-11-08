#include "parser.hpp"

#include "snir/core/print.hpp"

#include <regex>

namespace snir {

auto Parser::parseModule(std::string const& source) -> std::optional<Module>
{
    auto pattern = std::regex(R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})");
    auto search  = std::string::const_iterator(source.cbegin());
    auto matches = std::smatch{};

    auto module = Module{};
    while (std::regex_search(search, source.cend(), matches, pattern)) {
        auto const type      = matches[1].str();
        auto const name      = matches[2].str();
        auto const arguments = matches[3].str();
        auto const body      = matches[4].str();
        snir::println("define {} {}({}) {{{}}}\n", type, name, arguments, body);
        search = matches.suffix().first;  // Update the search start position
    }

    return std::nullopt;
}

auto Parser::parseInstruction(std::string const& source) -> std::optional<Instruction>
{
    return std::nullopt;
}

auto Parser::parseType(std::string_view source) -> std::optional<Type>
{
#define SNIR_BUILTIN_TYPE(Identifier, Name)                                                          \
    if (source == std::string_view{#Name}) {                                                         \
        return Type::Identifier;                                                                     \
    }
#include "snir/ir/type.def"
#undef SNIR_BUILTIN_TYPE

    return std::nullopt;
}

}  // namespace snir
