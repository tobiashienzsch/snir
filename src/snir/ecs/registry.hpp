#pragma once

#include "snir/core/unreachable.hpp"
#include "snir/ecs/storage.hpp"
#include "snir/ecs/view.hpp"

#include <map>
#include <memory>
#include <typeindex>

namespace snir {

template<typename Id>
struct Registry
{
    Registry() = default;

    [[nodiscard]] auto create() -> Id
    {
        auto id = _nextId;
        _nextId = Id{int(_nextId) + 1};
        return id;
    }

    template<typename Component, typename... Args>
    auto emplace(Id id, Args&&... args) -> decltype(auto)
    {
        return assure<Component>()->emplace(unsigned(id), std::forward<Args>(args)...);
    }

    template<typename Component, typename... Func>
    auto patch(Id id, Func&&... func) -> decltype(auto)
    {
        return assure<Component>()->patch(unsigned(id), std::forward<Func>(func)...);
    }

    template<typename... Components>
    auto view() -> View<Id, Components...>
    {
        return View<Id, Components...>{std::tuple{assure<Components>()...}};
    }

private:
    template<typename Comp>
    [[nodiscard]] auto assure() -> StorageTypeFor<Id, Comp>*
    {
        if (auto comp = _comps.find(typeid(Comp)); comp != _comps.end()) {
            return static_cast<StorageTypeFor<Id, Comp>*>(comp->second.get());
        }

        auto comp = std::make_unique<StorageTypeFor<Id, Comp>>();
        auto* ptr = comp.get();
        _comps.emplace(typeid(Comp), std::move(comp));
        return ptr;
    }

    Id _nextId{};
    std::map<std::type_index, std::unique_ptr<StorageBase>> _comps;
};

}  // namespace snir
