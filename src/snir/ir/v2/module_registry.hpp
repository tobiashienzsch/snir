#pragma once

#include "snir/ir/v2/inst.hpp"
#include "snir/ir/v2/value.hpp"

namespace snir::v2 {

struct ModuleRegistry
{
    ModuleRegistry() = default;

    [[nodiscard]] auto create(InstKind kind) -> Inst;
    [[nodiscard]] auto create(ValueKind kind) -> Value;

    [[nodiscard]] auto getInstRegistry() -> InstRegistry&;
    [[nodiscard]] auto getInstRegistry() const -> InstRegistry const&;

    [[nodiscard]] auto getValueRegistry() -> ValueRegistry&;
    [[nodiscard]] auto getValueRegistry() const -> ValueRegistry const&;

private:
    InstRegistry _instRegistry;
    ValueRegistry _valueRegistry;
};

}  // namespace snir::v2
