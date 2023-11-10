#include "snir/ir/v2/registry.hpp"

namespace snir::v2 {

auto Registry::create(InstKind kind) -> Inst
{
    auto inst = Inst::create(getInstRegistry());
    inst.emplace<InstKind>(kind);
    return inst;
}

auto Registry::create(ValueKind kind) -> Value
{
    auto val = Value::create(getValueRegistry());
    val.emplace<ValueKind>(kind);
    return val;
}

auto Registry::getInstRegistry() -> InstRegistry& { return _instRegistry; }

auto Registry::getInstRegistry() const -> InstRegistry const& { return _instRegistry; }

auto Registry::getValueRegistry() -> ValueRegistry& { return _valueRegistry; }

auto Registry::getValueRegistry() const -> ValueRegistry const& { return _valueRegistry; }

}  // namespace snir::v2
