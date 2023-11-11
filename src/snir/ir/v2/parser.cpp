#include "parser.hpp"

#include "snir/core/print.hpp"
#include "snir/core/strings.hpp"
#include "snir/ir/v2/compare_kind.hpp"

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

auto parseInstKind(std::string_view source) -> InstKind
{
    static constexpr auto names = std::array{
#define SNIR_INSTRUCTION(Id, Name) InstName{InstKind::Id, std::string_view{#Name}},
#include "snir/ir/v2/inst_kind.def"
#undef SNIR_INSTRUCTION
    };

    auto found = std::ranges::find(names, strings::trim(source), &InstName::name);
    if (found == std::ranges::end(names)) {
        raisef<std::invalid_argument>("failed to parse '{}' as InstKind", source);
    }

    return found->kind;
}

[[nodiscard]] auto parseType(std::string_view source) -> Type
{
#define SNIR_TYPE(Id, Name)                                                                          \
    if (source == std::string_view{#Name}) {                                                         \
        return Type::Id;                                                                             \
    }
#include "snir/ir/v2/type.def"
#undef SNIR_TYPE

    raisef<std::invalid_argument>("failed to parse '{}' as Type", source);
}

[[nodiscard]] auto parseCompareKind(std::string_view src) -> CompareKind
{
#define SNIR_COMPARE(Op, Name)                                                                       \
    if (src == std::string_view{#Name}) {                                                            \
        return CompareKind::Op;                                                                      \
    }
#include "snir/ir/v2/compare_kind.def"
#undef SNIR_COMPARE

    raisef<std::invalid_argument>("failed to parse '{}' as CompareKind", src);
}

auto parseLiteral(std::string_view src, Type type) -> Literal
{
    if (auto const m = ctre::match<R"(([\d]+(|\.[\d]+)))">(src); m) {
        if (type == Type::Int64) {
            return Literal{strings::parse<std::int64_t>(m.get<1>())};
        }
        if (type == Type::Float) {
            return Literal{strings::parse<float>(m.get<1>())};
        }
        if (type == Type::Double) {
            return Literal{strings::parse<double>(m.get<1>())};
        }
    }

    raisef<std::invalid_argument>("failed to parse '{}' as Literal", src);
}

enum struct IdentifierKind
{
    Global,
    Local,
};

[[nodiscard]] auto parseIdentifier(std::string_view src)
    -> std::pair<IdentifierKind, std::string_view>
{
    if (auto const m = ctre::match<R"(([%@][a-zA-Z0-9$._]+))">(src); m) {
        auto str = m.get<1>().view();
        if (str.starts_with("%")) {
            return {IdentifierKind::Local, str.substr(1)};
        }
        return {IdentifierKind::Global, str.substr(1)};
    }

    raisef<std::invalid_argument>("failed to parse '{}' as Operand", src);
}

}  // namespace

Parser::Parser(Registry& module) : _module{&module} {}

auto Parser::read(std::string_view source) -> std::string
{
    try {
        for (auto m : ctre::range<R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})">(source)) {
            _locals.clear();

            auto func = _module->create(ValueKind::Function);
            func.emplace<Type>(parseType(m.get<1>()));
            func.emplace<Name>(m.get<2>().to_string());
            func.emplace<FuncArguments>(readArguments(m.get<3>()));
            func.emplace<FuncBody>(readBlocks(m.get<4>()));

            _module->getFunctions().push_back(func.getId());
        }
    } catch (std::exception const& e) {
        return e.what();
    }

    return {};
}

auto Parser::readArguments(std::string_view source) -> std::vector<ValueId>
{
    auto args = std::vector<ValueId>{};
    for (auto match : ctre::range<R"(\s*([a-zA-Z_]\w*)\s+%([0-9]+)(?:\s*,\s*|$))">(source)) {
        auto local = getOrCreateLocal(match.get<2>(), ValueKind::Register);
        local.emplace<Type>(parseType(match.get<1>()));
        args.push_back(local.getId());
    }
    return args;
}

auto Parser::readInst(std::string_view source) -> std::optional<Inst>
{
    if (auto inst = readBinaryInst(source); inst) {
        return inst;
    }

    if (auto inst = readIntCmpInst(source); inst) {
        return inst;
    }

    if (auto inst = readTruncInst(source); inst) {
        return inst;
    }

    if (auto inst = readReturnInst(source); inst) {
        return inst;
    }

    if (auto inst = readBranchInst(source); inst) {
        return inst;
    }

    if (auto inst = readConstInst(source); inst) {
        return inst;
    }

    if (strings::contains(source, "; nop")) {
        auto inst = _module->create(InstKind::Nop);
        inst.emplace<Type>(Type::Void);
        return inst;
    }

    raisef<std::runtime_error>("failed to parse '{}' as an instruction", source);
}

auto Parser::readBlocks(std::string_view source) -> std::vector<BasicBlock>
{
    auto blocks = std::vector<BasicBlock>{};

    for (auto match : ctre::split<R"(\d+:)">(source)) {
        auto const str = strings::trim(match, " \t\n");
        if (str.empty()) {
            continue;
        }

        blocks.push_back(readBlock(str));
    }

    return blocks;
}

auto Parser::readBlock(std::string_view source) -> BasicBlock
{
    auto block = BasicBlock{.label = _module->create(ValueKind::Label).getId(), .instructions = {}};
    for (auto match : ctre::split<R"(\n)">(source)) {
        if (auto inst = readInst(strings::trim(match, " \t")); inst) {
            block.instructions.push_back(inst->getId());
        }
    }
    return block;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto Parser::readBinaryInst(std::string_view source) -> std::optional<Inst>
{
    // ([%@][a-zA-Z0-9$._]+)
    if (auto match = ctre::match<R"(%(\d+)\s+=\s+(\w+)\s+(\w+)\s+%(\d+),\s+%(\d+))">(source); match) {
        auto kind   = parseInstKind(match.get<2>());
        auto type   = parseType(match.get<3>());
        auto result = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto lhs    = getOrCreateLocal(match.get<4>(), ValueKind::Register);
        auto rhs    = getOrCreateLocal(match.get<5>(), ValueKind::Register);

        auto inst = _module->create(kind);
        inst.emplace<Type>(type);
        inst.emplace<Result>(Result{result});
        inst.emplace<Operands>(StaticVector<ValueId, 2>{lhs, rhs});
        return inst;
    }

    return std::nullopt;
}

auto Parser::readConstInst(std::string_view source) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(%(\d+)\s+=\s+(\w+)\s+(\d+\.\d+|\d+))">(source); match) {
        auto const result  = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto const type    = parseType(match.get<2>());
        auto const literal = parseLiteral(match.get<3>(), type);

        auto inst = _module->create(InstKind::Const);
        inst.emplace<Type>(type);
        inst.emplace<Result>(Result{result});
        inst.emplace<Literal>(literal);
        return inst;
    }

    return std::nullopt;
}

auto Parser::readIntCmpInst(std::string_view src) -> std::optional<Inst>
{
    // <result> = icmp eq i32 4, 5
    auto match = ctre::match<R"(%(\d+)\s+=\s+icmp\s+(\w+)\s+(\w+)\s+%(\d+),\s+%(\d+))">(src);
    if (match) {
        auto const result = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto const cmp    = parseCompareKind(match.get<2>());
        auto const type   = parseType(match.get<3>());
        auto const lhs    = getOrCreateLocal(match.get<4>(), ValueKind::Register);
        auto const rhs    = getOrCreateLocal(match.get<5>(), ValueKind::Register);

        auto inst = _module->create(InstKind::IntCmp);
        inst.emplace<Type>(type);
        inst.emplace<Result>(result.getId());
        inst.emplace<CompareKind>(cmp);
        inst.emplace<Operands>(StaticVector<ValueId, 2>{lhs.getId(), rhs.getId()});
        return inst;
    }

    return std::nullopt;
}

auto Parser::readTruncInst(std::string_view src) -> std::optional<Inst>
{
    // %2 = trunc %1 to float
    if (auto match = ctre::match<R"(%(\d+)\s+=\s+trunc\s+%(\d+)\s+to\s+(\w+))">(src); match) {
        auto const result = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto const value  = getOrCreateLocal(match.get<2>(), ValueKind::Register);
        auto const type   = parseType(match.get<3>());

        auto inst = _module->create(InstKind::Trunc);
        inst.emplace<Type>(type);
        inst.emplace<Result>(result);
        inst.emplace<Operands>(StaticVector<ValueId, 2>{value});
        return inst;
    }

    return std::nullopt;
}

auto Parser::readReturnInst(std::string_view src) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(ret\s+(\w+)\s+(\S+))">(src); match) {
        auto const type            = parseType(match.get<1>());
        auto const [opKind, opSrc] = parseIdentifier(match.get<2>());
        auto const operand         = getOrCreateLocal(opSrc, ValueKind::Register);

        auto ret = _module->create(InstKind::Return);
        ret.emplace<Type>(type);
        ret.emplace<Operands>(StaticVector<ValueId, 2>{operand.getId()});
        return ret;
    }

    if (auto match = ctre::match<R"(ret\s+(\w+))">(src); match) {
        if (match.get<1>() == "void") {
            auto ret = _module->create(InstKind::Return);
            ret.emplace<Type>(Type::Void);
            ret.emplace<Operands>();
            return ret;
        }
    }

    return std::nullopt;
}

auto Parser::readBranchInst(std::string_view src) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(br\s+label\s+(\S+))">(src); match) {
        auto const iftrue = getOrCreateLocal(match.get<1>().view().substr(1), ValueKind::Label);

        auto br = _module->create(InstKind::Branch);
        br.emplace<Type>(Type::Bool);
        br.emplace<Result>(ValueId{-1});
        br.emplace<Operands>();
        br.emplace<Branch>(Branch{std::nullopt, iftrue, std::nullopt});
        return br;
    }

    if (auto match = ctre::match<R"(br\s+i1\s+%(\d+),\s+label\s+%(\d+),\s+label\s+%(\d+))">(src);
        match) {
        auto const condition = getOrCreateLocal(match.get<1>(), ValueKind::Register);
        auto const iftrue    = getOrCreateLocal(match.get<2>(), ValueKind::Label);
        auto const iffalse   = getOrCreateLocal(match.get<3>(), ValueKind::Label);

        auto br = _module->create(InstKind::Branch);
        br.emplace<Type>(Type::Bool);
        br.emplace<Result>(ValueId{-1});
        br.emplace<Operands>();
        br.emplace<Branch>(Branch{condition, iftrue, iffalse});
        return br;
    }

    return std::nullopt;
}

auto Parser::getOrCreateLocal(std::string_view token, ValueKind kind) -> Value
{
    if (token.starts_with('%') or token.starts_with('@')) {
        raisef<std::runtime_error>("invalid token prefix '{}' for value", token);
    }

    if (auto found = _locals.find(token); found != _locals.end()) {
        return Value{_module->getValues(), found->second};
    }

    auto const value = _module->create(kind);
    _locals.emplace(token, value.getId());
    return value;
}

}  // namespace snir::v2
