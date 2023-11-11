#include "snir/ir/v2/registry.hpp"

namespace snir::v2 {

auto Registry::create(InstKind kind) -> Inst
{
    auto inst = Inst::create(getInsts());
    inst.emplace<InstKind>(kind);
    return inst;
}

auto Registry::create(ValueKind kind) -> Value
{
    auto val = Value::create(getValues());
    val.emplace<ValueKind>(kind);
    return val;
}

auto Registry::getInsts() -> InstRegistry& { return _instRegistry; }

auto Registry::getInsts() const -> InstRegistry const& { return _instRegistry; }

auto Registry::getValues() -> ValueRegistry& { return _valueRegistry; }

auto Registry::getValues() const -> ValueRegistry const& { return _valueRegistry; }

}  // namespace snir::v2
