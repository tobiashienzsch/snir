#include "Interpreter.hpp"

#include "snir/ir/Branch.hpp"
#include "snir/ir/CompareKind.hpp"
#include "snir/ir/InstKind.hpp"
#include "snir/ir/Operands.hpp"
#include "snir/ir/Result.hpp"
#include "snir/ir/Type.hpp"
#include "snir/ir/ValueKind.hpp"

#include "snir/core/print.hpp"
#include "snir/core/unreachable.hpp"

namespace snir {

auto Interpreter::execute(Function const& func, std::span<ValueId const> args)
    -> std::optional<Literal>
{
    _registers.clear();

    if (func.getArguments().size() != args.size()) {
        return std::nullopt;
    }

    auto const* registry = func.getValue().registry();

    auto currentBlock = func.getBasicBlocks().at(0).label;

    auto instructions = registry->view<InstKind, Type>();
    auto operands     = registry->view<Operands>();
    auto result       = registry->view<Result>();
    auto literal      = registry->view<Literal>();
    auto compare      = registry->view<CompareKind>();
    auto branch       = registry->view<Branch>();

    auto shiftLeft = []<typename T>(T lhs, T rhs) {
        return static_cast<T>(std::uint64_t(lhs) << std::uint64_t(rhs));
    };
    auto shiftRight = []<typename T>(T lhs, T rhs) {
        return static_cast<T>(std::uint64_t(lhs) >> std::uint64_t(rhs));
    };

    auto executeConst = [&](ValueId inst) {
        auto const [res] = result.get(inst);
        auto const [val] = literal.get(inst);
        _registers.emplace(res.id, val);
    };

    auto executeTrunc = [&](ValueId inst, Type type) {
        auto const [res] = result.get(inst);
        auto const [ops] = operands.get(inst);
        auto const in    = _registers.at(ops.list[0]);
        if (type == Type::Int64) {
            auto toInt64 = [](auto v) { return static_cast<std::int64_t>(v); };
            _registers.emplace(res.id, std::visit(toInt64, in.value));
            return;
        }
        if (type == Type::Float) {
            auto toFloat = [](auto v) { return static_cast<float>(v); };
            _registers.emplace(res.id, std::visit(toFloat, in.value));
            return;
        }
        if (type == Type::Double) {
            auto toDouble = [](auto v) { return static_cast<double>(v); };
            _registers.emplace(res.id, std::visit(toDouble, in.value));
            return;
        }

        raisef<std::runtime_error>("unsupported type {} for trunc op", type);
    };

    auto executeBinaryOp = [&](ValueId inst, Type type, auto op) {
        auto const [res] = result.get(inst);
        auto const [ops] = operands.get(inst);
        auto const lhs   = _registers.at(ops.list[0]).value;
        auto const rhs   = _registers.at(ops.list[1]).value;
        if (type == Type::Int64) {
            auto val = op(std::get<std::int64_t>(lhs), std::get<std::int64_t>(rhs));
            _registers.emplace(res.id, val);
            return;
        }

        raisef<std::runtime_error>("unsupported type {} for integer op", type);
    };

    auto executeBinaryFloatOp = [&](ValueId inst, Type type, auto op) {
        auto const [res] = result.get(inst);
        auto const [ops] = operands.get(inst);
        auto const lhs   = _registers.at(ops.list[0]).value;
        auto const rhs   = _registers.at(ops.list[1]).value;
        if (type == Type::Float) {
            auto val = op(std::get<float>(lhs), std::get<float>(rhs));
            _registers.emplace(res.id, val);
            return;
        }
        if (type == Type::Double) {
            auto val = op(std::get<double>(lhs), std::get<double>(rhs));
            _registers.emplace(res.id, val);
            return;
        }

        raisef<std::runtime_error>("unsupported type {} for float op", type);
    };

    auto executeIntCmp = [&](ValueId inst) {
        auto const [res] = result.get(inst);
        auto const [ops] = operands.get(inst);
        auto const [cmp] = compare.get(inst);
        auto const lhs   = _registers.at(ops.list[0]).value;
        auto const rhs   = _registers.at(ops.list[1]).value;
        if (cmp == CompareKind::Equal) {
            _registers.emplace(res.id, static_cast<bool>(lhs == rhs));
            return;
        }
        if (cmp == CompareKind::NotEqual) {
            _registers.emplace(res.id, static_cast<bool>(lhs != rhs));
            return;
        }

        raisef<std::runtime_error>("unsupported kind {} for integer compare", cmp);
    };

    auto executeReturn = [&](ValueId inst, Type type) {
        if (type == Type::Void) {
            return Literal{std::nan("")};
        }

        return _registers.at(std::get<0>(operands.get(inst)).list[0]);
    };

    auto executeBranch = [&](ValueId inst) {
        auto [dest]  = branch.get(inst);
        currentBlock = dest.iftrue;
    };

    while (true) {
        auto const& blocks = func.getBasicBlocks();
        auto const block   = std::ranges::find(blocks, currentBlock, &BasicBlock::label);
        if (block == std::ranges::end(blocks)) {
            raisef<std::runtime_error>("unknown block");
        }

        for (auto const inst : block->instructions) {
            auto const [kind, type] = instructions.get(inst);
            switch (kind) {
                case InstKind::Nop: break;
                case InstKind::Const: executeConst(inst); break;
                case InstKind::Trunc: executeTrunc(inst, type); break;
                case InstKind::Add: executeBinaryOp(inst, type, std::plus{}); break;
                case InstKind::Sub: executeBinaryOp(inst, type, std::minus{}); break;
                case InstKind::Mul: executeBinaryOp(inst, type, std::multiplies{}); break;
                case InstKind::Div: executeBinaryOp(inst, type, std::divides{}); break;
                case InstKind::Mod: executeBinaryOp(inst, type, std::modulus{}); break;
                case InstKind::And: executeBinaryOp(inst, type, std::bit_and{}); break;
                case InstKind::Or: executeBinaryOp(inst, type, std::bit_or{}); break;
                case InstKind::Xor: executeBinaryOp(inst, type, std::bit_xor{}); break;
                case InstKind::ShiftLeft: executeBinaryOp(inst, type, shiftLeft); break;
                case InstKind::ShiftRight: executeBinaryOp(inst, type, shiftRight); break;
                case InstKind::FloatAdd: executeBinaryFloatOp(inst, type, std::plus{}); break;
                case InstKind::FloatSub: executeBinaryFloatOp(inst, type, std::minus{}); break;
                case InstKind::FloatMul: executeBinaryFloatOp(inst, type, std::multiplies{}); break;
                case InstKind::FloatDiv: executeBinaryFloatOp(inst, type, std::divides{}); break;
                case InstKind::IntCmp: executeIntCmp(inst); break;
                case InstKind::Return: return executeReturn(inst, type);
                case InstKind::Branch: executeBranch(inst); break;
                case InstKind::Phi:
                default: raisef<std::runtime_error>("unimplemented: {}<{}>", kind, type);
            }
        }

        if (auto const next = std::ranges::next(block); next != std::ranges::end(blocks)) {
            currentBlock = next->label;
        } else {
            raisef<std::runtime_error>("reached on of function without return");
        }
    }

    return std::nullopt;
}

}  // namespace snir
