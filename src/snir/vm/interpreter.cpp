#include "interpreter.hpp"

#include "snir/vm/virtual_machine.hpp"

namespace snir {

auto Interpreter::execute(Function const& func, std::span<Value const> arguments)
    -> std::optional<Value>
{
    if (func.arguments.size() != arguments.size()) {
        return std::nullopt;
    }

    auto vm = VirtualMachine{func, arguments};
    return vm.getReturnValue();
}

}  // namespace snir
