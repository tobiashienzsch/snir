#include "PassManager.hpp"

#include "snir/core/print.hpp"

#include <chrono>

namespace snir {

PassManager::PassManager(bool log, std::ostream& out) : _out{out}, _log{log} {}

auto PassManager::operator()(Module& m) -> void
{
    for (auto& funcId : m.getFunctions()) {
        auto func = Function(Value{m.getRegistry(), funcId});
        std::invoke(*this, func, _analysis);
    }
}

auto PassManager::operator()(Function& func, AnalysisManager<Function>& analysis) -> void
{
    for (auto& pass : _passes) {
        auto const start = std::chrono::steady_clock::now();
        pass->run(func, analysis);
        auto const stop  = std::chrono::steady_clock::now();
        auto const delta = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        if (_log) {
            println(
                _out,
                "; function pass on {}: {} ({})",
                func.getIdentifier(),
                pass->getName(),
                delta
            );
        }
    }
}

}  // namespace snir
