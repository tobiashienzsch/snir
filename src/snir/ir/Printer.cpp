#include "Printer.hpp"

#include "snir/core/Print.hpp"
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

#include <algorithm>
#include <iterator>

namespace snir {

[[nodiscard]] auto getPredsForBlock(ControlFlowGraph::Result const& result, ValueId block)
    -> std::vector<ValueId>
{
    auto const node  = result.nodeIds[block];
    auto const edges = result.graph.getInEdges(node);

    auto preds = std::vector<ValueId>{};
    preds.reserve(edges.size());
    for (auto const edge : edges) {
        preds.push_back(result.nodeIds[edge.source]);
    }
    return preds;
};

Printer::Printer(std::ostream& out) : _out{out} {}

auto Printer::operator()(Module& module) -> void
{
    auto& reg  = module.getRegistry();
    auto dummy = AnalysisManager<Function>{};

    for (auto funcId : module.getFunctions()) {
        auto func = Function(Value{reg, funcId});
        (*this)(func, dummy);
    }
}

auto Printer::operator()(Function& function, AnalysisManager<Function>& analysis) -> void
{
    _cfg = &analysis.getResult<ControlFlowGraph>(function);
    _localIds.clear();
    printFunction(function);
}

auto Printer::printFunction(Function& func) -> void
{
    auto& reg = *func.getValue().registry();
    auto view = reg.view<Type, Identifier, FunctionDefinition>();

    auto const [type, identifier, def] = view.get(func.getValue());

    print(_out, "define {} @{}", type, identifier.text);
    printFunctionArgs(func);

    println(_out, " {{");
    auto const& blocks = func.getBasicBlocks();
    for (auto i{0U}; i < blocks.size(); ++i) {
        printBasicBlock(func, blocks.at(i));
        if (auto const isLast = i == blocks.size() - 1U; not isLast) {
            println(_out, "");
        }
    }
    println(_out, "}}\n");
}

auto Printer::printFunctionArgs(Function& func) -> void
{
    auto const& args = func.getArguments();
    auto types       = func.getValue().registry()->view<Type>();

    if (args.empty()) {
        print(_out, "()");
        return;
    }

    auto const a0 = std::ranges::begin(args);
    auto const l  = std::ranges::end(args);
    print(_out, "({} %{}", std::get<0>(types.get(*a0)), _localIds.add(*a0));
    std::ranges::for_each(std::ranges::next(a0), l, [this, &types](ValueId arg) {
        print(_out, ", {} %{}", std::get<0>(types.get(arg)), _localIds.add(arg));
    });
    print(_out, ")");
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
            return std::format("%{}", _localIds.add(val));
        }
        return std::format("{}", int(val));
    };

    print(_out, "{}:", _localIds.add(block.label));
    if (_cfg != nullptr) {
        auto const preds = getPredsForBlock(*_cfg, block.label);
        if (not preds.empty()) {
            auto first = std::ranges::begin(preds);
            print(_out, "\t\t\t\t\t\t; preds = %{}", _localIds.add(*first));
            std::ranges::for_each(
                std::ranges::next(first),
                std::ranges::end(preds),
                [this](auto pred) { print(_out, ", %{}", _localIds.add(pred)); }
            );
        }
    }
    println(_out, "");

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
                    println(_out, "  {} label %{}", kind, _localIds.add(br.iftrue));
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

}  // namespace snir
