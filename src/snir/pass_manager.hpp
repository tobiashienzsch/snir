#pragma once

#include "snir/function.hpp"
#include "snir/print.hpp"

#include <functional>
#include <memory>

namespace snir {

struct PassManager
{
    static constexpr auto name = std::string_view{"PassManager"};

    explicit PassManager(bool print = false) : _print{print} {}

    template<typename PassType>
    auto add(PassType&& p) -> void
    {
        _passes.emplace_back(std::make_unique<Pass<PassType>>(std::forward<PassType>(p)));
    }

    auto operator()(Function& func) -> void
    {
        for (auto& pass : _passes) {
            if (_print) {
                std::println("function pass on {}: {}", func.name, pass->getName());
            }
            pass->run(func);
        }
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

        [[nodiscard]] virtual auto getName() const -> std::string = 0;
        virtual auto run(Function& func) -> void                  = 0;
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

        auto run(Function& func) -> void override { std::invoke(_pass, func); }

    private:
        Type _pass;
    };

    std::vector<std::unique_ptr<PassInterface>> _passes;
    bool _print;
};

}  // namespace snir
