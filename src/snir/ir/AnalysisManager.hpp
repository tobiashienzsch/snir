#pragma once

#include "snir/ir/Function.hpp"

namespace snir {

template<typename IRUnitT>
struct AnalysisManager
{
    AnalysisManager() = default;

    template<typename PassT>
    [[nodiscard]] auto getResult(IRUnitT& unit) -> typename PassT::Result&
    {
        using ResultT = typename PassT::Result;

        auto val     = unit.asValue();
        auto* result = val.template try_get<ResultT>();
        if (result != nullptr) {
            return *result;
        }

        auto pass = PassT{};
        return val.template emplace<ResultT>(pass(unit, *this));
    }
};

}  // namespace snir
