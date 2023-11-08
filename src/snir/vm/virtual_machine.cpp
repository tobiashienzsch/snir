#include "virtual_machine.hpp"

#include "snir/core/print.hpp"

namespace snir {

VirtualMachine::VirtualMachine(Function const& func, std::span<Value const> arguments)
{
    auto id = 0;
    for (auto arg : arguments) {
        _register.emplace(Register{id++}, arg);
    }

    for (auto const& block : func.blocks) {
        for (auto const& inst : block) {
            // println("Exec: '{}'", inst.visit([](auto i) { return i.name; }));
            inst.visit(*this);
            if (_exit) {
                return;
            }
        }
    }
}

auto VirtualMachine::getReturnValue() const -> std::optional<Value> { return _return; }

auto VirtualMachine::operator()(ReturnInst const& inst) -> void
{
    if (std::holds_alternative<Register>(inst.value)) {
        _return = _register.at(std::get<Register>(inst.value));
    } else {
        _return = inst.value;
    }

    _exit = true;
}

auto VirtualMachine::operator()(ConstInst const& inst) -> void
{
    _register.emplace(inst.result, inst.value);
}

auto VirtualMachine::operator()(AddInst const& inst) -> void
{
    return binaryIntegerInst<std::plus<>>(inst);
}

auto VirtualMachine::operator()(SubInst const& inst) -> void
{
    return binaryIntegerInst<std::minus<>>(inst);
}

auto VirtualMachine::operator()(MulInst const& inst) -> void
{
    return binaryIntegerInst<std::multiplies<>>(inst);
}

auto VirtualMachine::operator()(DivInst const& inst) -> void
{
    return binaryIntegerInst<std::divides<>>(inst);
}

auto VirtualMachine::operator()(ModInst const& inst) -> void
{
    return binaryIntegerInst<std::modulus<>>(inst);
}

// auto VirtualMachine::operator()(FloatAddInst const& inst) -> void
// {
//     return binaryIntegerInst<std::plus<>>(inst);
// }

// auto VirtualMachine::operator()(FloatSubInst const& inst) -> void
// {
//     return binaryIntegerInst<std::plus<>>(inst);
// }

// auto VirtualMachine::operator()(FloatMulInst const& inst) -> void
// {
//     return binaryIntegerInst<std::plus<>>(inst);
// }

// auto VirtualMachine::operator()(FloatDivInst const& inst) -> void
// {
//     return binaryIntegerInst<std::plus<>>(inst);
// }

auto VirtualMachine::operator()(AndInst const& inst) -> void
{
    return binaryIntegerInst<std::bit_and<>>(inst);
}

auto VirtualMachine::operator()(OrInst const& inst) -> void
{
    return binaryIntegerInst<std::bit_or<>>(inst);
}

auto VirtualMachine::operator()(XorInst const& inst) -> void
{
    return binaryIntegerInst<std::bit_xor<>>(inst);
}

auto VirtualMachine::operator()(ShiftLeftInst const& inst) -> void
{
    auto shiftLeft = [](auto lhs, auto rhs) { return lhs << rhs; };
    return binaryIntegerInst<decltype(shiftLeft)>(inst);
}

auto VirtualMachine::operator()(ShiftRightInst const& inst) -> void
{
    auto shiftRight = [](auto lhs, auto rhs) { return lhs << rhs; };
    return binaryIntegerInst<decltype(shiftRight)>(inst);
}

}  // namespace snir
