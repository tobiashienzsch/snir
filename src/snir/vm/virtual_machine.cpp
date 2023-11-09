#include "virtual_machine.hpp"

#include "snir/core/print.hpp"

namespace snir {

VirtualMachine::VirtualMachine(Function const& func, std::span<Value const> arguments)
{
    auto id = 0;
    for (auto arg : arguments) {
        _register.emplace(Register{id++}, arg);
    }

    if (func.blocks.empty()) {
        return;
    }

    auto i = 0UL;
    while (true) {
        auto const& block = func.blocks[i];
        for (auto const& inst : block) {
            inst.visit(*this);
            if (_exit) {
                return;
            }
        }

        if (_nextBlock) {
            i          = *_nextBlock;
            _nextBlock = std::nullopt;
        } else {
            ++i;
        }
    }
}

auto VirtualMachine::getReturnValue() const -> std::optional<Value> { return _return; }

auto VirtualMachine::operator()(NopInst const& /*inst*/) -> void {}

auto VirtualMachine::operator()(BranchInst const& inst) -> void
{
    if (not inst.condition.has_value()) {
        _nextBlock = static_cast<size_t>(inst.iftrue);
        return;
    }

    raisef<std::runtime_error>("unimplemented conditional branch 'br'");
}

auto VirtualMachine::operator()(ReturnInst const& inst) -> void
{
    if (auto const* reg = std::get_if<Register>(&inst.value); reg != nullptr) {
        _return = _register.at(*reg);
    } else {
        _return = inst.value;
    }

    _exit = true;
}

auto VirtualMachine::operator()(ConstInst const& inst) -> void
{
    _register.emplace(inst.result, inst.value);
}

auto VirtualMachine::operator()(TruncInst const& inst) -> void
{
    if (inst.type == Type::Int64) {
        _register.emplace(inst.result, std::visit(CastTo<int>{_register}, inst.value));
        return;
    }
    if (inst.type == Type::Float) {
        _register.emplace(inst.result, std::visit(CastTo<float>{_register}, inst.value));
        return;
    }
    if (inst.type == Type::Double) {
        _register.emplace(inst.result, std::visit(CastTo<double>{_register}, inst.value));
        return;
    }

    raisef<std::runtime_error>("unsupported type for trunc instruction: '{}'", inst.type);
}

auto VirtualMachine::operator()(AddInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::plus{});
}

auto VirtualMachine::operator()(SubInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::minus{});
}

auto VirtualMachine::operator()(MulInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::multiplies{});
}

auto VirtualMachine::operator()(DivInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::divides{});
}

auto VirtualMachine::operator()(ModInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::modulus{});
}

auto VirtualMachine::operator()(AndInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::bit_and{});
}

auto VirtualMachine::operator()(OrInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::bit_or{});
}

auto VirtualMachine::operator()(XorInst const& inst) -> void
{
    return binaryIntegerInst(inst, std::bit_xor{});
}

auto VirtualMachine::operator()(ShiftLeftInst const& inst) -> void
{
    return binaryIntegerInst(inst, []<typename T>(T lhs, T rhs) {
        return static_cast<T>(std::uint64_t(lhs) << std::uint64_t(rhs));
    });
}

auto VirtualMachine::operator()(ShiftRightInst const& inst) -> void
{
    return binaryIntegerInst(inst, []<typename T>(T lhs, T rhs) {
        return static_cast<T>(std::uint64_t(lhs) >> std::uint64_t(rhs));
    });
}

auto VirtualMachine::operator()(IntCmpInst const& inst) -> void
{
    return binaryIntegerInst(inst, [kind = inst.kind]<typename T>(T lhs, T rhs) {
        switch (kind) {
            case Compare::Equal: return static_cast<T>(lhs == rhs);
            case Compare::NotEqual: return static_cast<T>(lhs != rhs);
        }
        raisef<std::runtime_error>("unimplemented compare kind '{}'", kind);
    });
}

auto VirtualMachine::operator()(FloatAddInst const& inst) -> void
{
    return binaryFloatInst(inst, std::plus{});
}

auto VirtualMachine::operator()(FloatSubInst const& inst) -> void
{
    return binaryFloatInst(inst, std::minus{});
}

auto VirtualMachine::operator()(FloatMulInst const& inst) -> void
{
    return binaryFloatInst(inst, std::multiplies{});
}

auto VirtualMachine::operator()(FloatDivInst const& inst) -> void
{
    return binaryFloatInst(inst, std::divides{});
}

}  // namespace snir
