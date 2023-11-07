#pragma once

#include "snir/ir/function.hpp"
#include "snir/ir/value.hpp"

#include <map>
#include <optional>
#include <span>

namespace snir {

struct VirtualMachine
{
    explicit VirtualMachine(Function const& func, std::span<Value> arguments);

    [[nodiscard]] auto getReturnValue() const -> std::optional<Value>;

    auto operator()(ReturnInst const& inst) -> void;
    auto operator()(ConstInst const& inst) -> void;

    auto operator()(auto const& inst) -> void {}

private:
    std::map<Register, Value> _registers;
    std::optional<Value> _return{Value{std::nullopt}};
    bool _exit{false};
};

struct Interpreter
{
    Interpreter() = default;

    static auto execute(Function const& func, std::span<Value> arguments) -> std::optional<Value>;

private:
};

}  // namespace snir
