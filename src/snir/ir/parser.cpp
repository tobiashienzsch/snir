#include "parser.hpp"

#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

#include <ctre.hpp>

#include <regex>

namespace snir {

auto Parser::readModule(std::string const& src) -> std::optional<Module>
{
    auto module = Module{};

    auto matches = std::smatch{};
    auto search  = std::string::const_iterator(src.cbegin());
    auto pattern = std::regex(R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})");

    while (std::regex_search(search, src.cend(), matches, pattern)) {
        auto const type = readType(matches[1].str());
        if (not type) {
            return std::nullopt;
        }

        auto const arguments = readFunctionArgs(matches[3].str());
        if (not arguments) {
            return std::nullopt;
        }

        auto const blocks = readBasicBlocks(matches[4].str());
        if (not blocks) {
            return std::nullopt;
        }

        module.functions.push_back(Function{
            .type      = type.value(),
            .name      = matches[2].str(),
            .arguments = arguments.value(),
            .blocks    = blocks.value(),
        });

        // Update the search start position
        search = matches.suffix().first;
    }

    return module;
}

auto Parser::readInstruction(std::string const& src) -> std::optional<Instruction>
{
    if (auto inst = readBinaryInst(src); inst) {
        return inst;
    }

    if (auto inst = readConstInst(src); inst) {
        return inst.value();
    }

    if (auto inst = readTruncInst(src); inst) {
        return inst.value();
    }

    if (auto inst = readIntCmpInst(src); inst) {
        return inst.value();
    }

    if (auto inst = readReturnInst(src); inst) {
        return inst.value();
    }

    if (strings::contains(src, "; nop")) {
        return NopInst{};
    }

    return std::nullopt;
}

auto Parser::readType(std::string_view src) -> std::optional<Type>
{
#define SNIR_BUILTIN_TYPE(Id, Name)                                                                  \
    if (src == std::string_view{#Name}) {                                                            \
        return Type::Id;                                                                             \
    }
#include "snir/ir/def/type.def"
#undef SNIR_BUILTIN_TYPE

    return std::nullopt;
}

auto Parser::readFunctionArgs(std::string const& src) -> std::optional<std::vector<Type>>
{
    auto args = std::vector<Type>{};

    auto matches = std::smatch{};
    auto search  = std::string::const_iterator(src.cbegin());
    auto pattern = std::regex(R"(\s*([a-zA-Z_]\w*)\s+%[0-9]+(?:\s*,\s*|$))");

    while (std::regex_search(search, src.cend(), matches, pattern)) {
        auto const type = readType(matches[1].str());
        if (not type) {
            return std::nullopt;
        }

        args.push_back(type.value());
        search = matches.suffix().first;
    }

    return args;
}

auto Parser::readBasicBlocks(std::string const& src) -> std::optional<std::vector<BasicBlock>>
{
    auto blocks = std::vector<BasicBlock>{};

    auto pattern = std::regex(R"(\d+:)");
    auto it      = std::sregex_token_iterator(src.begin(), src.end(), pattern, -1);
    auto end     = std::sregex_token_iterator{};
    for (; it != end; ++it) {
        auto const str = strings::trim(it->str(), " \t\n");
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

auto Parser::readBasicBlock(std::string const& src) -> std::optional<BasicBlock>
{
    auto block = BasicBlock{};

    auto line   = std::string{};
    auto stream = std::istringstream(src);
    while (std::getline(stream, line)) {
        auto inst = readInstruction(strings::trim(line));
        if (inst) {
            block.push_back(inst.value());
        }
    }

    return block;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto Parser::readBinaryInst(std::string_view src) -> std::optional<Instruction>
{
    if (auto match = ctre::match<R"(%(\d+) = (\w+) (\w+) (\d+|%\d+) (\d+|%\d+))">(src); match) {
        auto op     = match.get<2>().to_view();
        auto type   = readType(match.get<3>().to_view()).value();
        auto result = Register{std::stoi(match.get<1>().to_string())};
        auto lhs    = readValue(match.get<4>().to_view(), type).value();
        auto rhs    = readValue(match.get<5>().to_view(), type).value();

#define SNIR_INST_BINARY_OP(Id, Name)                                                                \
    if (op == std::string_view{#Name}) {                                                             \
        return Id##Inst{                                                                             \
            .type   = type,                                                                          \
            .result = result,                                                                        \
            .lhs    = lhs,                                                                           \
            .rhs    = rhs,                                                                           \
        };                                                                                           \
    }

#include "snir/ir/def/binary_op.def"
#undef SNIR_INST_BINARY_OP
    }

    return std::nullopt;
}

auto Parser::readConstInst(std::string_view src) -> std::optional<ConstInst>
{
    if (auto match = ctre::match<R"(%(\d+) = (\w+) (\d+\.\d+|\d+))">(src); match) {
        auto const result = Register{std::stoi(match.get<1>().to_string())};
        auto const type   = readType(match.get<2>().to_view()).value();
        auto const value  = readValue(match.get<3>().to_view(), type).value();

        return ConstInst{
            .type   = type,
            .result = result,
            .value  = value,
        };
    }

    return std::nullopt;
}

auto Parser::readIntCmpInst(std::string_view src) -> std::optional<IntCmpInst>
{
    // <result> = icmp eq i32 4, 5
    if (auto match = ctre::match<R"(%(\d+) = icmp (\w+) (\w+) %(\d+) %(\d+))">(src); match) {
        auto const result = Register{std::stoi(match.get<1>().to_string())};
        auto const kind   = readCompare(match.get<2>().to_view()).value();
        auto const type   = readType(match.get<3>().to_view()).value();
        auto const lhs    = Register{std::stoi(match.get<4>().to_string())};
        auto const rhs    = Register{std::stoi(match.get<5>().to_string())};

        return IntCmpInst{
            .type   = type,
            .kind   = kind,
            .result = result,
            .lhs    = lhs,
            .rhs    = rhs,
        };
    }

    return std::nullopt;
}

auto Parser::readTruncInst(std::string_view src) -> std::optional<TruncInst>
{
    // %2 = trunc %1 as float
    if (auto match = ctre::match<R"(%(\d+) = trunc %(\d+) as (\w+))">(src); match) {
        auto const result = Register{std::stoi(match.get<1>().to_string())};
        auto const value  = Register{std::stoi(match.get<2>().to_string())};
        auto const type   = readType(match.get<3>().to_view()).value();
        return TruncInst{
            .type   = type,
            .result = result,
            .value  = value,
        };
    }

    return std::nullopt;
}

auto Parser::readReturnInst(std::string_view src) -> std::optional<ReturnInst>
{
    if (auto match = ctre::match<R"(ret (\w+) %(\d+))">(src); match) {
        auto const type    = readType(match.get<1>()).value();
        auto const operand = std::stoi(match.get<2>().to_string());
        return ReturnInst{.type = type, .value = Register{operand}};
    }

    return std::nullopt;
}

auto Parser::readCompare(std::string_view src) -> std::optional<Compare>
{
#define SNIR_INST_COMPARE_OP(Op, Name)                                                               \
    if (src == std::string_view{#Name}) {                                                            \
        return Compare::Op;                                                                          \
    }
#include "snir/ir/def/compare_op.def"
#undef SNIR_INST_COMPARE_OP

    return std::nullopt;
}

auto Parser::readValue(std::string_view src, Type type) -> std::optional<Value>
{
    if (auto const m = ctre::match<R"((%[0-9]+)|([\d]+(|\.[\d]+)))">(src); m) {
        if (m.get<1>()) {
            return Register{std::stoi(m.get<1>().to_string().substr(1))};
        } else if (m.get<2>()) {
            auto const str = m.get<2>().to_string();
            if (type == Type::Int64) {
                return std::stoi(str);
            }
            if (type == Type::Float) {
                return std::stof(str);
            }
            if (type == Type::Double) {
                return std::stod(str);
            }
        }
    }

    return std::nullopt;
}

}  // namespace snir
