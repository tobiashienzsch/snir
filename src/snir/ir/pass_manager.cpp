#include "pass_manager.hpp"

#include "snir/core/print.hpp"

#include <chrono>

namespace snir {

PassManager::PassManager(bool log, std::ostream& out) : _out{out}, _log{log} {}

auto PassManager::operator()(Module& m) -> void
{
    for (auto& func : m.functions) {
        std::invoke(*this, func);
    }
}

auto PassManager::operator()(Function& func) -> void
{
    for (auto& pass : _passes) {
        auto const start = std::chrono::steady_clock::now();
        pass->run(func);
        auto const stop  = std::chrono::steady_clock::now();
        auto const delta = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        if (_log) {
            println(_out, "function pass on {}: {} ({})", func.name, pass->getName(), delta);
        }
    }
}

}  // namespace snir
