#include "snir/ir/v2/module.hpp"

namespace snir::v2 {

auto Module::create(InstKind kind) -> Inst
{
    auto inst = Inst::create(getInsts());
    inst.emplace<InstKind>(kind);
    return inst;
}

auto Module::create(ValueKind kind) -> Value
{
    auto val = Value::create(getValues());
    val.emplace<ValueKind>(kind);
    return val;
}

auto Module::getInsts() -> InstRegistry& { return _instRegistry; }

auto Module::getInsts() const -> InstRegistry const& { return _instRegistry; }

auto Module::getValues() -> ValueRegistry& { return _valueRegistry; }

auto Module::getValues() const -> ValueRegistry const& { return _valueRegistry; }

}  // namespace snir::v2
