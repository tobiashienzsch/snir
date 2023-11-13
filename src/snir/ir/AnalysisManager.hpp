#pragma once

namespace snir {

template<typename IRUnitT>
struct AnalysisManager
{
    AnalysisManager() = default;

    template<typename PassT>
    [[nodiscard]] auto getResult(IRUnitT& unit) -> PassT::Result&;
};

}  // namespace snir
