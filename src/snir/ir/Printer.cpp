#include "Printer.hpp"

#include "snir/ir/Branch.hpp"
#include "snir/ir/CompareKind.hpp"
#include "snir/ir/Identifier.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Literal.hpp"
#include "snir/ir/Operands.hpp"
#include "snir/ir/Result.hpp"
#include "snir/ir/Type.hpp"
#include "snir/ir/Value.hpp"
#include "snir/ir/ValueKind.hpp"

#include "snir/core/print.hpp"

#include <algorithm>
#include <iterator>

namespace snir::v3 {

Printer::Printer(std::ostream& out) : _out{out} {}

auto Printer::operator()(Module& module) -> void
{
    auto& reg = module.getRegistry();

    for (auto funcId : module.getFunctions()) {
        auto func = Function(Value{reg, funcId});
        (*this)(func);
    }
}

auto Printer::operator()(Function& function) -> void
{
    _nextLocalValueId = 0;
    _localValueIds.clear();

    auto& reg = *function.getValue().registry();
    auto view = reg.view<Type, Identifier, FunctionDefinition>();

    auto const [type, identifier, def] = view.get(function.getValue());
    print(_out, "define {} @{}", type, identifier.text);
    printFunction(function);
    println(_out, "");
}

auto Printer::printFunction(Function& func) -> void
{
    if (func.getArguments().empty()) {
        print(_out, "()");
    } else {
        auto types = func.getValue().registry()->view<Type>();
        auto a0    = func.getArguments().at(0);

        auto printArg = [this, &types](ValueId arg) {
            print(_out, ", {} %{}", std::get<0>(types.get(arg)), getLocalId(arg));
        };

        print(_out, "({} %{}", std::get<0>(types.get(a0)), getLocalId(a0));
        std::ranges::for_each(
            std::ranges::next(std::ranges::begin(func.getArguments())),
            std::ranges::end(func.getArguments()),
            printArg
        );
        print(_out, ")");
    }

    println(_out, " {{");
    for (auto const& block : func.getBasicBlocks()) {
        printBasicBlock(func, block);
    }
    println(_out, "}}");
}

auto Printer::printBasicBlock(Function& func, BasicBlock const& block) -> void
{
    auto& reg      = *func.getValue().registry();
    auto common    = reg.view<InstKind, Type>();
    auto result    = reg.view<Result>();
    auto operands  = reg.view<Operands>();
    auto compare   = reg.view<CompareKind>();
    auto literal   = reg.view<Literal>();
    auto branch    = reg.view<Branch>();
    auto valueKind = reg.view<ValueKind>();

    auto formatValue = [&](ValueId val) {
        auto [kind] = valueKind.get(val);
        if (kind == ValueKind::Register) {
            return std::format("%{}", getLocalId(val));
        }
        return std::format("{}", int(val));
    };

    println(_out, "{}:", getLocalId(block.label));

    for (auto const inst : block.instructions) {
        auto const [kind, type] = common.get(inst);
        switch (kind) {
            case InstKind::Nop: {
                println(_out, "  ; {}", kind);
                break;
            }
            case InstKind::Const: {
                auto const [id]    = result.get(inst);
                auto const res     = formatValue(id.id);
                auto const [value] = literal.get(inst);
                println(_out, "  {} = {} {}", res, type, value);
                break;
            }
            case InstKind::Return: {
                if (type == Type::Void) {
                    println(_out, "  {} {}", kind, type);
                } else {
                    auto const [args] = operands.get(inst);
                    auto const value  = formatValue(args.list[0]);
                    println(_out, "  {} {} {}", kind, type, value);
                }
                break;
            }
            case InstKind::Branch: {
                auto [br] = branch.get(inst);
                if (not br.condition) {
                    println(_out, "  {} label {}", kind, getLocalId(br.iftrue));
                } else {
                }
                break;
            }
            case InstKind::Phi: {
                println(_out, "  {} ", kind);
                break;
            }
            case InstKind::Add:
            case InstKind::Sub:
            case InstKind::Mul:
            case InstKind::Div:
            case InstKind::Mod:
            case InstKind::And:
            case InstKind::Or:
            case InstKind::Xor:
            case InstKind::ShiftLeft:
            case InstKind::ShiftRight:
            case InstKind::FloatAdd:
            case InstKind::FloatSub:
            case InstKind::FloatMul:
            case InstKind::FloatDiv: {
                auto const [id]   = result.get(inst);
                auto const [args] = operands.get(inst);
                auto const res    = formatValue(id.id);
                auto const lhs    = formatValue(args.list[0]);
                auto const rhs    = formatValue(args.list[1]);
                println(_out, "  {} = {} {} {}, {}", res, kind, type, lhs, rhs);
                break;
            }

            case InstKind::IntCmp: {
                auto const [id]   = result.get(inst);
                auto const [args] = operands.get(inst);
                auto const res    = formatValue(id.id);
                auto const [cmp]  = compare.get(inst);
                auto const lhs    = formatValue(args.list[0]);
                auto const rhs    = formatValue(args.list[1]);
                println(_out, "  {} = {} {} {} {}, {}", res, kind, cmp, type, lhs, rhs);
                break;
            }
            case InstKind::Trunc: {
                auto const [id]   = result.get(inst);
                auto const [args] = operands.get(inst);
                auto const res    = formatValue(id.id);
                auto const value  = formatValue(args.list[0]);
                println(_out, "  {} = {} {} to {}", res, kind, value, type);
                break;
            }
        }
    }
}

auto Printer::getLocalId(ValueId value) -> int
{
    if (auto found = _localValueIds.find(value); found != _localValueIds.end()) {
        return found->second;
    }

    auto id = _nextLocalValueId++;
    _localValueIds.emplace(value, id);
    return id;
}

}  // namespace snir::v3
