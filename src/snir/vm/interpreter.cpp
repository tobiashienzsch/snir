#include "interpreter.hpp"

#include "snir/core/print.hpp"

namespace snir {

VirtualMachine::VirtualMachine(Function const& func, std::span<Value> arguments)
{
    auto id = 0;
    for (auto arg : arguments) {
        _registers.emplace(Register{id++}, arg);
    }

    for (auto const& block : func.blocks) {
        for (auto const& inst : block) {
            println("{}", inst.visit([](auto i) { return i.name; }));
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
        _return = _registers.at(std::get<Register>(inst.value));
    } else {
        _return = inst.value;
    }

    _exit = true;
}

auto VirtualMachine::operator()(ConstInst const& inst) -> void
{
    _registers.emplace(inst.destination, inst.value);
}

auto Interpreter::execute(Function const& func, std::span<Value> arguments) -> std::optional<Value>
{
    if (func.arguments.size() != arguments.size()) {
        return std::nullopt;
    }

    auto vm = VirtualMachine{func, arguments};
    return vm.getReturnValue();
}

}  // namespace snir
