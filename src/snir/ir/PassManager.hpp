#pragma once

#include "snir/ir/AnalysisManager.hpp"
#include "snir/ir/Function.hpp"
#include "snir/ir/Module.hpp"

#include <functional>
#include <iostream>
#include <memory>

namespace snir {

struct PassManager
{
    static constexpr auto name = std::string_view{"PassManager"};

    explicit PassManager(bool log = false, std::ostream& out = std::cout);

    auto operator()(Module& m) -> void;
    auto operator()(Function& func, AnalysisManager<Function>& analysis) -> void;

    template<typename PassType>
    auto add(PassType&& p) -> void
    {
        _passes.emplace_back(std::make_unique<Pass<PassType>>(std::forward<PassType>(p)));
    }

private:
    struct PassInterface
    {
        PassInterface()          = default;
        virtual ~PassInterface() = default;

        PassInterface(PassInterface const&)                    = delete;
        auto operator=(PassInterface const&) -> PassInterface& = delete;

        PassInterface(PassInterface&&)                    = delete;
        auto operator=(PassInterface&&) -> PassInterface& = delete;

        [[nodiscard]] virtual auto getName() const -> std::string                     = 0;
        virtual auto run(Function& func, AnalysisManager<Function>& analysis) -> void = 0;
    };

    template<typename Type>
    struct Pass final : PassInterface
    {
        explicit Pass(Type p) : _pass(std::move(p)) {}

        [[nodiscard]] auto getName() const -> std::string override
        {
            if constexpr (requires { Type::name; }) {
                return std::string{Type::name};
            } else {
                return std::string{Type::type::name};
            }
        }

        auto run(Function& func, AnalysisManager<Function>& analysis) -> void override
        {
            std::invoke(_pass, func, analysis);
        }

    private:
        Type _pass;
    };

    AnalysisManager<Function> _analysis;
    std::vector<std::unique_ptr<PassInterface>> _passes;
    std::reference_wrapper<std::ostream> _out;
    bool _log;
};

}  // namespace snir
