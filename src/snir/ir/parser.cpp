#include "parser.hpp"

#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"

#include <ctre.hpp>

namespace snir {

auto Parser::readModule(std::string_view src) -> std::optional<Module>
{
    auto module = Module{};

    for (auto match : ctre::range<R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})">(src)) {
        auto const type = readType(match.get<1>().to_view());
        if (not type) {
            return std::nullopt;
        }

        auto const arguments = readFunctionArgs(match.get<3>().to_string());
        if (not arguments) {
            return std::nullopt;
        }

        auto const blocks = readBasicBlocks(match.get<4>().to_string());
        if (not blocks) {
            return std::nullopt;
        }

        module.functions.push_back(Function{
            .type      = type.value(),
            .name      = match.get<2>().to_string(),
            .arguments = arguments.value(),
            .blocks    = blocks.value(),
        });
    }

    return module;
}

auto Parser::readInstruction(std::string_view src) -> std::optional<Instruction>
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

    if (auto inst = readBranchInst(src); inst) {
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

auto Parser::readFunctionArgs(std::string_view src) -> std::optional<std::vector<Type>>
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

auto Parser::readBasicBlocks(std::string_view src) -> std::optional<std::vector<BasicBlock>>
{
    auto blocks = std::vector<BasicBlock>{};

    for (auto match : ctre::split<R"(\d+:)">(src)) {
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

auto Parser::readBasicBlock(std::string_view src) -> std::optional<BasicBlock>
{
    auto block = BasicBlock{};

    for (auto match : ctre::split<R"(\n)">(src)) {
        auto inst = readInstruction(strings::trim(match, " \t"));
        if (inst) {
            block.push_back(inst.value());
        }
    }

    return block;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto Parser::readBinaryInst(std::string_view src) -> std::optional<Instruction>
{
    if (auto match = ctre::match<R"(%(\d+) = (\w+) (\w+) (\d+|%\d+), (\d+|%\d+))">(src); match) {
        auto op     = match.get<2>().to_view();
        auto type   = readType(match.get<3>().to_view()).value();
        auto result = Register{strings::parse<int>(match.get<1>())};
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
        auto const result = Register{strings::parse<int>(match.get<1>())};
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
    if (auto match = ctre::match<R"(%(\d+) = icmp (\w+) (\w+) %(\d+), %(\d+))">(src); match) {
        auto const result = Register{strings::parse<int>(match.get<1>())};
        auto const kind   = readCompare(match.get<2>().to_view()).value();
        auto const type   = readType(match.get<3>().to_view()).value();
        auto const lhs    = Register{strings::parse<int>(match.get<4>())};
        auto const rhs    = Register{strings::parse<int>(match.get<5>())};

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
    // %2 = trunc %1 to float
    if (auto match = ctre::match<R"(%(\d+) = trunc %(\d+) to (\w+))">(src); match) {
        auto const result = Register{strings::parse<int>(match.get<1>())};
        auto const value  = Register{strings::parse<int>(match.get<2>())};
        auto const type   = readType(match.get<3>().to_view()).value();
        return TruncInst{
            .type   = type,
            .result = result,
            .value  = value,
        };
    }

    return std::nullopt;
}

auto Parser::readBranchInst(std::string_view src) -> std::optional<BranchInst>
{
    if (auto match = ctre::match<R"(br i1 %(\d+), label %(\d+), label %(\d+))">(src); match) {
        auto const condition = Register{strings::parse<int>(match.get<1>())};
        auto const iftrue    = Label{strings::parse<int>(match.get<2>())};
        auto const iffalse   = Label{strings::parse<int>(match.get<3>())};
        return BranchInst{.iftrue = iftrue, .iffalse = iffalse, .condition = condition};
    }

    if (auto match = ctre::match<R"(br label %(\d+))">(src); match) {
        auto const iftrue = Label{strings::parse<int>(match.get<1>())};
        return BranchInst{.iftrue = iftrue, .iffalse = std::nullopt, .condition = std::nullopt};
    }

    return std::nullopt;
}

auto Parser::readReturnInst(std::string_view src) -> std::optional<ReturnInst>
{
    if (auto match = ctre::match<R"(ret (\w+) %(\d+))">(src); match) {
        auto const type    = readType(match.get<1>()).value();
        auto const operand = Register{strings::parse<int>(match.get<2>())};
        return ReturnInst{.type = type, .value = operand};
    }

    if (auto match = ctre::match<R"(ret (\w+))">(src); match) {
        if (match.get<1>() == "void") {
            return ReturnInst{.type = Type::Void, .value = Void{}};
        }
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
            return Register{strings::parse<int>(m.get<1>().to_view().substr(1))};
        }
        if (m.get<2>()) {
            if (type == Type::Int64) {
                return strings::parse<int>(m.get<2>());
            }
            if (type == Type::Float) {
                return strings::parse<float>(m.get<2>());
            }
            if (type == Type::Double) {
                return strings::parse<double>(m.get<2>());
            }
        }
    }

    return std::nullopt;
}

}  // namespace snir
