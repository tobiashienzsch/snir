#pragma once

#include "snir/ir/v2/module.hpp"
#include "snir/ir/v2/value.hpp"

#include <algorithm>
#include <iterator>
#include <ostream>

namespace snir::v2 {

struct Printer
{
    explicit Printer(std::ostream& out) : _out{out} {}

    auto operator()(Module& module) -> void
    {
        auto& vals = module.getRegistry().getValues();
        auto view  = vals.view<Type, Name, FunctionDefinition>();

        for (auto funcId : module.getFunctions()) {
            _nextLocalValueId = 0;

            auto func = Value{vals, funcId};

            auto const [type, name, funcDefintion] = view.get(func.getId());
            print(_out, "define {} @{}", type, name.text);
            (*this)(module, funcDefintion);
            println(_out, "");
        }
    }

    auto operator()(Module& module, FunctionDefinition const& func) -> void
    {
        if (func.args.empty()) {
            return print(_out, "()");
        }

        auto types = module.getRegistry().getValues().view<Type>();
        auto a0    = func.args.at(0);

        print(_out, "({} %{}", types.get(a0), getLocalId(a0));
        std::ranges::for_each(
            std::ranges::next(std::ranges::begin(func.args)),
            std::ranges::end(func.args),
            [this, &types](auto arg) { print(_out, ", {} %{}", types.get(arg), getLocalId(arg)); }
        );
        print(_out, ")");

        println(_out, " {{");
        for (auto const& block : func.blocks) {
            (*this)(module, block);
        }
        println(_out, "}}");
    }

    auto operator()(Module& module, BasicBlock const& block) -> void
    {
        auto& insts   = module.getRegistry().getInsts();
        auto common   = insts.view<InstKind, Type>();
        auto result   = insts.view<Result>();
        auto operands = insts.view<Operands>();
        auto compare  = insts.view<CompareKind>();
        auto literal  = insts.view<Literal>();
        auto branch   = insts.view<Branch>();

        auto& values   = module.getRegistry().getValues();
        auto valueKind = values.view<ValueKind>();

        auto formatValue = [&](auto val) {
            auto kind = valueKind.get(val);
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
                    auto const res   = formatValue(result.get(inst).id);
                    auto const value = literal.get(inst);
                    println(_out, "  {} = {} {}", res, type, value);
                    break;
                }
                case InstKind::Return: {
                    if (type == Type::Void) {
                        println(_out, "  {} {}", kind, type);
                    } else {
                        auto const value = formatValue(operands.get(inst).list[0]);
                        println(_out, "  {} {} {}", kind, type, value);
                    }
                    break;
                }
                case InstKind::Branch: {
                    auto br = branch.get(inst);
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
                    auto const res = formatValue(result.get(inst).id);
                    auto const lhs = formatValue(operands.get(inst).list[0]);
                    auto const rhs = formatValue(operands.get(inst).list[1]);
                    println(_out, "  {} = {} {} {}, {}", res, kind, type, lhs, rhs);
                    break;
                }

                case InstKind::IntCmp: {
                    auto const res = formatValue(result.get(inst).id);
                    auto const cmp = compare.get(inst);
                    auto const lhs = formatValue(operands.get(inst).list[0]);
                    auto const rhs = formatValue(operands.get(inst).list[1]);
                    println(_out, "  {} = {} {} {} {}, {}", res, kind, cmp, type, lhs, rhs);
                    break;
                }
                case InstKind::Trunc: {
                    auto const res   = formatValue(result.get(inst).id);
                    auto const value = formatValue(operands.get(inst).list[0]);
                    println(_out, "  {} = {} {} to {}", res, kind, value, type);
                    break;
                }
            }
        }
    }

private:
    [[nodiscard]] auto getLocalId(ValueId value) -> int
    {
        if (auto found = _localValueIds.find(value); found != _localValueIds.end()) {
            return found->second;
        }

        auto id = _nextLocalValueId++;
        _localValueIds.emplace(value, id);
        return id;
    }

    std::reference_wrapper<std::ostream> _out;
    int _nextLocalValueId{0};
    std::map<ValueId, int> _localValueIds{};
};

}  // namespace snir::v2
