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
    if (auto const m = ctre::match<R"((%[0-9]+)|([\d]+(|\.[\d]+)))">(src); m) {
        if (m.get<1>()) {
            // return Register{strings::parse<int>(m.get<1>().substr(1))};
        }
        if (m.get<2>()) {
            if (type == Type::Int64) {
                return Literal{strings::parse<std::int64_t>(m.get<2>())};
            }
            if (type == Type::Float) {
                return Literal{strings::parse<float>(m.get<2>())};
            }
            if (type == Type::Double) {
                return Literal{strings::parse<double>(m.get<2>())};
            }
        }
    }

    raisef<std::invalid_argument>("failed to parse '{}' as Literal", src);
}

[[nodiscard]] auto parseArguments(std::string_view source) -> std::vector<Type>
{
    auto args = std::vector<Type>{};
    for (auto match : ctre::range<R"(\s*([a-zA-Z_]\w*)\s+%[0-9]+(?:\s*,\s*|$))">(source)) {
        args.push_back(parseType(match.get<1>()));
    }
    return args;
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto parseBinaryInst(Module& module, std::string_view source) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(%(\d+) = (\w+) (\w+) %(\d+), %(\d+))">(source); match) {
        auto kind   = parseInstKind(match.get<2>());
        auto type   = parseType(match.get<3>());
        auto result = strings::parse<int>(match.get<1>());
        auto lhs    = ValueId{strings::parse<int>(match.get<4>())};
        auto rhs    = ValueId{strings::parse<int>(match.get<5>())};

        auto inst = module.create(kind);
        inst.emplace<Type>(type);
        inst.emplace<Result>(Result{ValueId{result}});
        inst.emplace<Operands>(StaticVector<ValueId, 2>{lhs, rhs});
        return inst;
    }

    return std::nullopt;
}

auto parseConstInst(Module& module, std::string_view source) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(%(\d+) = (\w+) (\d+\.\d+|\d+))">(source); match) {
        auto const result  = strings::parse<int>(match.get<1>());
        auto const type    = parseType(match.get<2>());
        auto const literal = parseLiteral(match.get<3>(), type);

        auto inst = module.create(InstKind::Const);
        inst.emplace<Type>(type);
        inst.emplace<Result>(Result{ValueId{result}});
        inst.emplace<Literal>(literal);
        return inst;
    }

    return std::nullopt;
}

auto parseIntCmpInst(Module& module, std::string_view src) -> std::optional<Inst>
{
    // <result> = icmp eq i32 4, 5
    if (auto match = ctre::match<R"(%(\d+) = icmp (\w+) (\w+) %(\d+), %(\d+))">(src); match) {
        auto const result = ValueId{strings::parse<int>(match.get<1>())};
        auto const cmp    = parseCompareKind(match.get<2>());
        auto const type   = parseType(match.get<3>());
        auto const lhs    = ValueId{strings::parse<int>(match.get<4>())};
        auto const rhs    = ValueId{strings::parse<int>(match.get<5>())};

        auto inst = module.create(InstKind::IntCmp);
        inst.emplace<Type>(type);
        inst.emplace<Result>(result);
        inst.emplace<CompareKind>(cmp);
        inst.emplace<Operands>(StaticVector<ValueId, 2>{lhs, rhs});
        return inst;
    }

    return std::nullopt;
}

auto parseTruncInst(Module& module, std::string_view src) -> std::optional<Inst>
{
    // %2 = trunc %1 to float
    if (auto match = ctre::match<R"(%(\d+) = trunc %(\d+) to (\w+))">(src); match) {
        auto const result = ValueId{strings::parse<int>(match.get<1>())};
        auto const value  = ValueId{strings::parse<int>(match.get<2>())};
        auto const type   = parseType(match.get<3>());

        auto inst = module.create(InstKind::Trunc);
        inst.emplace<Type>(type);
        inst.emplace<Result>(result);
        inst.emplace<Operands>(StaticVector<ValueId, 2>{value});
        return inst;
    }

    return std::nullopt;
}

auto parseReturnInst(Module& module, std::string_view src) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(ret (\w+) %(\d+))">(src); match) {
        auto const type    = parseType(match.get<1>());
        auto const operand = ValueId{strings::parse<int>(match.get<2>())};

        auto ret = module.create(InstKind::Return);
        ret.emplace<Type>(type);
        ret.emplace<Operands>(StaticVector<ValueId, 2>{operand});
        return ret;
    }

    if (auto match = ctre::match<R"(ret (\w+))">(src); match) {
        if (match.get<1>() == "void") {
            auto ret = module.create(InstKind::Return);
            ret.emplace<Type>(Type::Void);
            ret.emplace<Operands>();
            return ret;
        }
    }

    return std::nullopt;
}

auto parseBranchInst(Module& module, std::string_view src) -> std::optional<Inst>
{
    if (auto match = ctre::match<R"(br label %(\d+))">(src); match) {
        auto const iftrue = ValueId{strings::parse<int>(match.get<1>())};

        auto br = module.create(InstKind::Branch);
        br.emplace<Type>(Type::Bool);
        br.emplace<Result>(ValueId{-1});
        br.emplace<Operands>();
        br.emplace<Branch>(Branch{std::nullopt, iftrue, std::nullopt});
        return br;
    }

    if (auto match = ctre::match<R"(br i1 %(\d+), label %(\d+), label %(\d+))">(src); match) {
        auto const condition = ValueId{strings::parse<int>(match.get<1>())};
        auto const iftrue    = ValueId{strings::parse<int>(match.get<2>())};
        auto const iffalse   = ValueId{strings::parse<int>(match.get<3>())};

        auto br = module.create(InstKind::Branch);
        br.emplace<Type>(Type::Bool);
        br.emplace<Result>(ValueId{-1});
        br.emplace<Operands>();
        br.emplace<Branch>(Branch{condition, iftrue, iffalse});
        return br;
    }

    return std::nullopt;
}

}  // namespace

auto Parser::readModule(std::string_view source) -> std::optional<Module>
{
    auto module = Module{};
    _current    = std::addressof(module);

    for (auto match : ctre::range<R"(define\s+(\w+)\s+@(\w+)\(([^)]*)\)\s*\{([^}]*)\})">(source)) {
        auto func = _current->create(ValueKind::Function);
        func.emplace<Type>(parseType(match.get<1>()));
        func.emplace<Name>(match.get<2>().to_string());
        func.emplace<FuncArguments>(parseArguments(match.get<3>()));

        auto const blocks = readBasicBlocks(match.get<4>().to_string());
        if (not blocks) {
            return std::nullopt;
        }

        auto& body  = func.emplace<FuncBody>(FuncBody{});
        body.blocks = *blocks;

        module.getFunctions().push_back(func.getId());
    }

    return module;
}

auto Parser::readInst(std::string_view source) -> std::optional<Inst>
{
    if (auto inst = parseBinaryInst(*_current, source); inst) {
        return inst;
    }

    if (auto inst = parseIntCmpInst(*_current, source); inst) {
        return inst;
    }

    if (auto inst = parseTruncInst(*_current, source); inst) {
        return inst;
    }

    if (auto inst = parseReturnInst(*_current, source); inst) {
        return inst;
    }

    if (auto inst = parseBranchInst(*_current, source); inst) {
        return inst;
    }

    if (auto inst = parseConstInst(*_current, source); inst) {
        return inst;
    }

    if (strings::contains(source, "; nop")) {
        auto inst = _current->create(InstKind::Nop);
        inst.emplace<Type>(Type::Void);
        return inst;
    }

    raisef<std::runtime_error>("failed to parse '{}' as an instruction", source);
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
        if (auto inst = readInst(strings::trim(match, " \t")); inst) {
            block.instructions.push_back(inst->getId());
        }
    }

    return block;
}

}  // namespace snir::v2
