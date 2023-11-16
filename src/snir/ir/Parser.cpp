#include "Parser.hpp"

#include "snir/core/Print.hpp"
#include "snir/core/Strings.hpp"
#include "snir/ir/Branch.hpp"
#include "snir/ir/CompareKind.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/FunctionDefinition.hpp"
#include "snir/ir/Identifier.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Instruction.hpp"
#include "snir/ir/Literal.hpp"
#include "snir/ir/Operands.hpp"
#include "snir/ir/Result.hpp"

#include <ctre.hpp>

#include <algorithm>
#include <array>

namespace snir {

Parser::Parser(Registry& registry) : _registry{&registry} {}

auto Parser::read(std::string_view source) -> Module
{
    auto module = Module{*_registry};

    for (auto match : ctre::range<R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})">(source)) {
        _locals.clear();

        auto func = Function::create(*_registry, parseType(match.get<1>()));
        func.setIdentifier(match.get<2>());
        func.getValue().emplace<FunctionDefinition>(
            readArguments(match.get<3>()),
            readBlocks(match.get<4>())
        );

        module.getFunctions().push_back(func);
    }

    return module;
}

auto Parser::readArguments(std::string_view source) -> std::vector<ValueId>
{
    auto args = std::vector<ValueId>{};
    for (auto match : ctre::range<R"(\s*([a-zA-Z_]\w*)\s+%([0-9]+)(?:\s*,\s*|$))">(source)) {
        auto local = getOrCreateLocal(match.get<2>(), ValueKind::Register);
        local.emplace<Type>(parseType(match.get<1>()));
        args.push_back(local);
    }
    return args;
}

auto Parser::readInst(std::string_view source) -> std::optional<ValueId>
{
    if (auto const inst = readBinaryInst(source); inst) {
        return inst;
    }

    if (auto const inst = readIntCmpInst(source); inst) {
        return inst;
    }

    if (auto const inst = readTruncInst(source); inst) {
        return inst;
    }

    if (auto const inst = readReturnInst(source); inst) {
        return inst;
    }

    if (auto const inst = readBranchInst(source); inst) {
        return inst;
    }

    if (auto const inst = readConstInst(source); inst) {
        return inst;
    }

    if (strings::contains(source, "; nop")) {
        return Instruction::create(*_registry, InstKind::Nop, Type::Void);
    }

    raisef<std::runtime_error>("failed to parse '{}' as an instruction", source);
}

auto Parser::readBlocks(std::string_view source) -> std::vector<BasicBlock>
{
    auto blocks  = std::vector<BasicBlock>{};
    auto current = std::optional<BasicBlock>{};

    strings::forEachLine(source, [&, this](std::string_view line) {
        auto const str = strings::trim(line);
        if (str.empty()) {
            return;
        }

        if (auto match = ctre::match<R"((\d+):)">(str); match) {
            auto token  = match.view();
            auto number = strings::removeSuffix(token, 1);
            if (current) {
                blocks.push_back(std::move(*current));
                current = std::nullopt;
            }

            current = BasicBlock{getOrCreateLocal(number, ValueKind::Label), {}};
            return;
        }

        if (auto const inst = readInst(strings::trim(line, " \t")); inst) {
            current->instructions.push_back(*inst);
        }
    });

    if (current) {
        blocks.push_back(std::move(*current));
    }

    return blocks;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto Parser::readBinaryInst(std::string_view source) -> std::optional<ValueId>
{
    // ([%@][a-zA-Z0-9$._]+)
    if (auto match = ctre::match<R"(%(\d+)\s+=\s+(\w+)\s+(\w+)\s+%(\d+),\s+%(\d+))">(source); match) {
        auto kind   = parseInstKind(match.get<2>());
        auto type   = parseType(match.get<3>());
        auto result = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto lhs    = getOrCreateLocal(match.get<4>(), ValueKind::Register);
        auto rhs    = getOrCreateLocal(match.get<5>(), ValueKind::Register);

        auto inst = Instruction::create(*_registry, kind, type);
        inst.getValue().emplace<Result>(result);
        inst.getValue().emplace<Operands>(StaticVector<ValueId, 2>{lhs, rhs});
        return inst;
    }

    return std::nullopt;
}

auto Parser::readIntCmpInst(std::string_view source) -> std::optional<ValueId>
{
    // <result> = icmp eq i32 4, 5
    auto match = ctre::match<R"(%(\d+)\s+=\s+icmp\s+(\w+)\s+(\w+)\s+%(\d+),\s+%(\d+))">(source);
    if (match) {
        auto const result = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto const cmp    = parseCompareKind(match.get<2>());
        auto const type   = parseType(match.get<3>());
        auto const lhs    = getOrCreateLocal(match.get<4>(), ValueKind::Register);
        auto const rhs    = getOrCreateLocal(match.get<5>(), ValueKind::Register);

        auto inst = Instruction::create(*_registry, InstKind::IntCmp, type);
        inst.getValue().emplace<Result>(result);
        inst.getValue().emplace<CompareKind>(cmp);
        inst.getValue().emplace<Operands>(StaticVector<ValueId, 2>{lhs, rhs});
        return inst;
    }

    return std::nullopt;
}

auto Parser::readTruncInst(std::string_view source) -> std::optional<ValueId>
{
    // %2 = trunc %1 to float
    if (auto match = ctre::match<R"(%(\d+)\s+=\s+trunc\s+%(\d+)\s+to\s+(\w+))">(source); match) {
        auto const result = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto const value  = getOrCreateLocal(match.get<2>(), ValueKind::Register);
        auto const type   = parseType(match.get<3>());

        auto inst = Instruction::create(*_registry, InstKind::Trunc, type);
        inst.getValue().emplace<Result>(result);
        inst.getValue().emplace<Operands>(StaticVector<ValueId, 2>{value});
        return inst;
    }

    return std::nullopt;
}

auto Parser::readReturnInst(std::string_view source) -> std::optional<ValueId>
{
    if (auto match = ctre::match<R"(ret\s+(\w+)\s+(\S+))">(source); match) {
        auto const type            = parseType(match.get<1>());
        auto const [opKind, opSrc] = parseIdentifier(match.get<2>());
        auto const operand         = getOrCreateLocal(opSrc, ValueKind::Register);

        auto ret = Instruction::create(*_registry, InstKind::Return, type);
        ret.getValue().emplace<Operands>(StaticVector<ValueId, 2>{operand});
        return ret;
    }

    if (auto match = ctre::match<R"(ret\s+(\w+))">(source); match) {
        if (match.get<1>() == "void") {
            auto ret = Instruction::create(*_registry, InstKind::Return, Type::Void);
            ret.getValue().emplace<Operands>();
            return ret;
        }
    }

    return std::nullopt;
}

auto Parser::readBranchInst(std::string_view source) -> std::optional<ValueId>
{
    if (auto m = ctre::match<R"(br\s+label\s+(\S+))">(source); m) {
        auto const iftrue = getOrCreateLocal(m.get<1>().view().substr(1), ValueKind::Label);

        auto br = Instruction::create(*_registry, InstKind::Branch, Type::Bool);
        br.getValue().emplace<Operands>();
        br.getValue().emplace<Branch>(iftrue, std::nullopt, std::nullopt);
        return br;
    }

    if (auto m = ctre::match<R"(br\s+i1\s+%(\d+),\s+label\s+%(\d+),\s+label\s+%(\d+))">(source); m) {
        auto const condition = getOrCreateLocal(m.get<1>(), ValueKind::Register);
        auto const iftrue    = getOrCreateLocal(m.get<2>(), ValueKind::Label);
        auto const iffalse   = getOrCreateLocal(m.get<3>(), ValueKind::Label);

        auto br = Instruction::create(*_registry, InstKind::Branch, Type::Bool);
        br.getValue().emplace<Operands>();
        br.getValue().emplace<Branch>(iftrue, iffalse, condition);
        return br;
    }

    return std::nullopt;
}

auto Parser::readConstInst(std::string_view source) -> std::optional<ValueId>
{
    if (auto match = ctre::match<R"(%(\d+)\s+=\s+(\w+)\s+(\d+\.\d+|\d+))">(source); match) {
        auto const result  = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto const type    = parseType(match.get<2>());
        auto const literal = parseLiteral(match.get<3>(), type);

        auto inst = Instruction::create(*_registry, InstKind::Const, type);
        inst.getValue().emplace<Result>(Result{result});
        inst.getValue().emplace<Literal>(literal);
        return inst;
    }

    return std::nullopt;
}

auto Parser::getOrCreateLocal(std::string_view token, ValueKind kind) -> Value
{
    if (token.starts_with('%') or token.starts_with('@')) {
        raisef<std::runtime_error>("invalid token prefix '{}' for value", token);
    }

    if (auto found = _locals.find(token); found != _locals.end()) {
        return Value{*_registry, found->second};
    }

    auto val = createValue(*_registry, kind);
    _locals.emplace(token, val);
    return val;
}

}  // namespace snir
