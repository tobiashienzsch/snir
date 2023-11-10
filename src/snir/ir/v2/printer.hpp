#pragma once

#include "snir/ir/v2/module.hpp"
#include "snir/ir/v2/value.hpp"

#include <ostream>

namespace snir::v2 {

struct Printer
{
    explicit Printer(std::ostream& out) : _out{out} {}

    auto operator()(Module& module) -> void
    {
        auto& vals = module.getValues();
        auto view  = vals.view<Type, Name, FuncArguments, FuncBody>();

        for (auto funcId : module.getFunctions()) {
            auto func = Value{vals, funcId};

            auto const [type, name, args, body] = view.get(func.getId());
            print(_out, "define {} @{}", type, name.text);
            (*this)(module, args);
            (*this)(module, body);
        }
    }

    auto operator()(Module&, FuncArguments const& args) -> void
    {
        if (args.args.empty()) {
            return print(_out, "()");
        }

        print(_out, "({}", args.args.at(0));
        for (auto arg : args.args) {
            print(_out, ", {}", arg);
        }
        print(_out, ")");
    }

    auto operator()(Module& mod, FuncBody const& body) -> void
    {
        println(_out, " {{");
        for (auto block : body.blocks) {
            (*this)(mod, block);
        }
        println(_out, "}}");
    }

    auto operator()(Module& mod, BasicBlock const& block) -> void
    {
        auto& reg     = mod.getInsts();
        auto view     = reg.view<InstKind, Type>();
        auto result   = reg.view<Result>();
        auto operands = reg.view<Operands>();
        auto compare  = reg.view<CompareKind>();
        auto literal  = reg.view<Literal>();
        auto branch   = reg.view<Branch>();

        println(_out, "{}:", int(block.label));

        for (auto const inst : block.instructions) {
            auto const [kind, type] = view.get(inst);
            switch (kind) {
                case InstKind::Nop: {
                    println(_out, "  ; {}", kind);
                    break;
                }
                case InstKind::Const: {
                    auto const res   = int(result.get(inst).id);
                    auto const value = literal.get(inst);
                    println(_out, "  %{} = {} {}", res, type, value);
                    break;
                }
                case InstKind::Return: {
                    if (type == Type::Void) {
                        println(_out, "  {} {}", kind, type);
                    } else {
                        auto const value = int(operands.get(inst).list[0]);
                        println(_out, "  {} {} {}", kind, type, value);
                    }
                    break;
                }
                case InstKind::Branch: {
                    auto br = branch.get(inst);
                    if (not br.condition) {
                        println(_out, "  {} label {}", kind, int(br.iftrue));
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
                    auto const res = int(result.get(inst).id);
                    auto const lhs = int(operands.get(inst).list[0]);
                    auto const rhs = int(operands.get(inst).list[1]);
                    println(_out, "  %{} = {} {} {}, {}", res, kind, type, lhs, rhs);
                    break;
                }

                case InstKind::IntCmp: {
                    auto const res = int(result.get(inst).id);
                    auto const cmp = compare.get(inst);
                    auto const lhs = int(operands.get(inst).list[0]);
                    auto const rhs = int(operands.get(inst).list[1]);
                    println(_out, "  %{} = {} {} {} {}, {}", res, kind, cmp, type, lhs, rhs);
                    break;
                }
                case InstKind::Trunc: {
                    auto const res   = int(result.get(inst).id);
                    auto const value = int(operands.get(inst).list[0]);
                    println(_out, "  %{} = {} {} to {}", res, kind, value, type);
                    break;
                }
            }
        }
    }

private:
    std::reference_wrapper<std::ostream> _out;
};

}  // namespace snir::v2
