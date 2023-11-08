#include "parser.hpp"

#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

#include <regex>

namespace snir {

auto Parser::parseModule(std::string const& source) -> std::optional<Module>
{
    auto module = Module{};

    auto matches = std::smatch{};
    auto search  = std::string::const_iterator(source.cbegin());
    auto pattern = std::regex(R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})");

    while (std::regex_search(search, source.cend(), matches, pattern)) {
        auto const type = parseType(matches[1].str());
        if (not type) {
            return std::nullopt;
        }

        auto const arguments = parseFunctionArguments(matches[3].str());
        if (not arguments) {
            return std::nullopt;
        }

        auto const blocks = parseBlocks(matches[4].str());
        if (not blocks) {
            return std::nullopt;
        }

        module.functions.push_back(Function{
            .type      = type.value(),
            .name      = matches[2].str(),
            .arguments = std::move(arguments.value()),
            .blocks    = std::move(blocks.value()),
        });

        // Update the search start position
        search = matches.suffix().first;
    }

    return module;
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

auto Parser::parseFunctionArguments(std::string const& source) -> std::optional<std::vector<Type>>
{
    auto args = std::vector<Type>{};

    auto matches = std::smatch{};
    auto search  = std::string::const_iterator(source.cbegin());
    auto pattern = std::regex(R"(\s*([a-zA-Z_]\w*)\s+%[0-9]+(?:\s*,\s*|$))");

    while (std::regex_search(search, source.cend(), matches, pattern)) {
        auto const type = parseType(matches[1].str());
        if (not type) {
            return std::nullopt;
        }

        args.push_back(type.value());
        search = matches.suffix().first;
    }

    return args;
}

auto Parser::parseBlocks(std::string const& source) -> std::optional<std::vector<Block>>
{
    auto blocks = std::vector<Block>{};

    auto pattern = std::regex(R"(\d+:)");
    auto it      = std::sregex_token_iterator(source.begin(), source.end(), pattern, -1);
    auto end     = std::sregex_token_iterator{};
    for (; it != end; ++it) {
        auto const str = strings::trim(it->str(), " \t\n");
        if (str.empty()) {
            continue;
        }
        blocks.push_back(Block{});
    }

    return blocks;
}

}  // namespace snir
