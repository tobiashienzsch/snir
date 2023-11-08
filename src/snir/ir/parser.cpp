#include "parser.hpp"

#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

#include <regex>

namespace snir {

auto Parser::parseModule(std::string const& src) -> std::optional<Module>
{
    auto module = Module{};

    auto matches = std::smatch{};
    auto search  = std::string::const_iterator(src.cbegin());
    auto pattern = std::regex(R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})");

    while (std::regex_search(search, src.cend(), matches, pattern)) {
        auto const type = parseType(matches[1].str());
        if (not type) {
            return std::nullopt;
        }

        auto const arguments = parseFunctionArgs(matches[3].str());
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

auto Parser::parseInstruction(std::string const& src) -> std::optional<Instruction>
{
    if (auto inst = parseBinaryInst(src); inst) {
        return inst;
    }

    if (auto inst = parseConstInst(src); inst) {
        return inst.value();
    }

    if (auto inst = parseTruncInst(src); inst) {
        return inst.value();
    }

    if (auto inst = parseReturnInst(src); inst) {
        return inst.value();
    }

    if (strings::contains(src, "; nop")) {
        return NopInst{};
    }

    return std::nullopt;
}

auto Parser::parseType(std::string_view src) -> std::optional<Type>
{
#define SNIR_BUILTIN_TYPE(Identifier, Name)                                                          \
    if (src == std::string_view{#Name}) {                                                            \
        return Type::Identifier;                                                                     \
    }
#include "snir/ir/type.def"
#undef SNIR_BUILTIN_TYPE

    return std::nullopt;
}

auto Parser::parseFunctionArgs(std::string const& src) -> std::optional<std::vector<Type>>
{
    auto args = std::vector<Type>{};

    auto matches = std::smatch{};
    auto search  = std::string::const_iterator(src.cbegin());
    auto pattern = std::regex(R"(\s*([a-zA-Z_]\w*)\s+%[0-9]+(?:\s*,\s*|$))");

    while (std::regex_search(search, src.cend(), matches, pattern)) {
        auto const type = parseType(matches[1].str());
        if (not type) {
            return std::nullopt;
        }

        args.push_back(type.value());
        search = matches.suffix().first;
    }

    return args;
}

auto Parser::parseBlocks(std::string const& src) -> std::optional<std::vector<Block>>
{
    auto blocks = std::vector<Block>{};

    auto pattern = std::regex(R"(\d+:)");
    auto it      = std::sregex_token_iterator(src.begin(), src.end(), pattern, -1);
    auto end     = std::sregex_token_iterator{};
    for (; it != end; ++it) {
        auto const str = strings::trim(it->str(), " \t\n");
        if (str.empty()) {
            continue;
        }

        auto block = parseBlock(str);
        if (not block) {
            return std::nullopt;
        }
        blocks.push_back(std::move(block.value()));
    }

    return blocks;
}

auto Parser::parseBlock(std::string const& src) -> std::optional<Block>
{
    auto block = Block{};

    auto line   = std::string{};
    auto stream = std::istringstream(src);
    while (std::getline(stream, line)) {
        auto inst = parseInstruction(strings::trim(line));
        if (inst) {
            block.push_back(inst.value());
        }
    }

    return block;
}

auto Parser::parseBinaryInst(std::string const& src) -> std::optional<Instruction>
{
    auto matches = std::smatch();
    auto pattern = std::regex(R"(%(\d+) = (\w+) (\w+) (\d+|%\d+) (\d+|%\d+))");
    if (std::regex_match(src, matches, pattern)) {
        auto op     = matches[2].str();
        auto type   = parseType(matches[3].str()).value();
        auto result = Register{std::stoi(matches[1])};
        auto lhs    = parseValue(matches[4], type).value();
        auto rhs    = parseValue(matches[5], type).value();

#define SNIR_INST_BINARY(Identifier, Name)                                                           \
    if (op == std::string_view{#Name}) {                                                             \
        return Identifier##Inst{                                                                     \
            .type   = type,                                                                          \
            .result = result,                                                                        \
            .lhs    = lhs,                                                                           \
            .rhs    = rhs,                                                                           \
        };                                                                                           \
    }

#include "snir/ir/instruction_binary.def"
#undef SNIR_INST_BINARY
    }

    return std::nullopt;
}

auto Parser::parseConstInst(std::string const& src) -> std::optional<ConstInst>
{
    auto matches = std::smatch();
    auto pattern = std::regex(R"(%(\d+) = (\w+) ([-+]?(\d+\.\d+|\d+)))");
    if (std::regex_match(src, matches, pattern)) {
        auto const result = Register{std::stoi(matches[1])};
        auto const type   = parseType(matches[2].str()).value();
        auto const value  = parseValue(matches[3], type).value();

        return ConstInst{
            .type   = type,
            .result = result,
            .value  = value,
        };
    }

    return std::nullopt;
}

auto Parser::parseTruncInst(std::string const& src) -> std::optional<TruncInst>
{
    // %2 = trunc %1 as float
    auto matches = std::smatch();
    auto pattern = std::regex(R"(%(\d+) = trunc %(\d+) as (\w+))");
    if (std::regex_match(src, matches, pattern)) {
        auto const result = Register{std::stoi(matches[1])};
        auto const type   = parseType(matches[3].str()).value();
        auto const value  = Register{std::stoi(matches[2])};
        return TruncInst{
            .type   = type,
            .result = result,
            .value  = value,
        };
    }

    return std::nullopt;
}

auto Parser::parseReturnInst(std::string const& src) -> std::optional<ReturnInst>
{
    auto matches = std::smatch();
    auto pattern = std::regex(R"(ret (\w+) %(\d+))");
    if (std::regex_match(src, matches, pattern)) {
        auto const type    = parseType(matches[1].str()).value();
        auto const operand = std::stoi(matches[2]);
        return ReturnInst{.type = type, .value = Register{operand}};
    }

    return std::nullopt;
}

auto Parser::parseValue(std::string const& src, Type type) -> std::optional<Value>
{
    if (src.starts_with("%")) {
        return Register{std::stoi(src.substr(1))};
    }

    if (type == Type::Int64) {
        return std::stoi(src);
    } else if (type == Type::Float) {
        return std::stof(src);
    } else {
        return std::stod(src);
    }

    return std::nullopt;
}

}  // namespace snir
