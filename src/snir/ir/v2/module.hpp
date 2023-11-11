#pragma once

#include "snir/ir/v2/inst.hpp"
#include "snir/ir/v2/value.hpp"

#include <vector>

namespace snir::v2 {

struct Module
{
    Module() = default;

    [[nodiscard]] auto create(InstKind kind) -> Inst;
    [[nodiscard]] auto create(ValueKind kind) -> Value;

    [[nodiscard]] auto getInsts() -> InstRegistry&;
    [[nodiscard]] auto getInsts() const -> InstRegistry const&;

    [[nodiscard]] auto getValues() -> ValueRegistry&;
    [[nodiscard]] auto getValues() const -> ValueRegistry const&;

    [[nodiscard]] auto getFunctions() -> std::vector<ValueId>& { return _functions; }

    [[nodiscard]] auto getFunctions() const -> std::vector<ValueId> const& { return _functions; }

private:
    InstRegistry _instRegistry;
    ValueRegistry _valueRegistry;
    std::vector<ValueId> _functions;
};

}  // namespace snir::v2
