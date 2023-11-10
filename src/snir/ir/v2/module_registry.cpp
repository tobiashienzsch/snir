#include "snir/ir/v2/module_registry.hpp"

namespace snir::v2 {

auto ModuleRegistry::create(InstKind kind) -> Inst
{
    auto inst = Inst::create(getInstRegistry());
    inst.emplace<InstKind>(kind);
    return inst;
}

auto ModuleRegistry::create(ValueKind kind) -> Value
{
    auto val = Value::create(getValueRegistry());
    val.emplace<ValueKind>(kind);
    return val;
}

auto ModuleRegistry::getInstRegistry() -> InstRegistry& { return _instRegistry; }

auto ModuleRegistry::getInstRegistry() const -> InstRegistry const& { return _instRegistry; }

auto ModuleRegistry::getValueRegistry() -> ValueRegistry& { return _valueRegistry; }

auto ModuleRegistry::getValueRegistry() const -> ValueRegistry const& { return _valueRegistry; }

}  // namespace snir::v2
