#pragma once

#include "snir/ecs/registry.hpp"

namespace snir {

template<typename Id>
struct Handle
{
    using IdType       = Id;
    using RegistryType = Registry<Id>;

    Handle() = default;

    Handle(Registry<Id>& registry, Id id) : _reg{&registry}, _id{id} {}

    [[nodiscard]] static auto create(Registry<Id>& reg) -> Handle { return {reg, reg.create()}; }

    [[nodiscard]] auto isValid() const noexcept -> bool { return _reg != nullptr; }

    [[nodiscard]] auto getRegistry() const noexcept -> Registry<Id>* { return _reg; }

    [[nodiscard]] auto getId() const noexcept -> Id { return _id; }

    template<typename Component>
    auto get() -> decltype(auto)
    {
        return _reg->template view<Component>().get(_id);
    }

    template<typename Component, typename... Args>
    auto emplace(Args&&... args) -> decltype(auto)
    {
        return _reg->template emplace<Component>(_id, std::forward<Args>(args)...);
    }

    template<typename Component, typename Func>
    auto patch(Func&& func) -> decltype(auto)
    {
        return _reg->template patch<Component>(_id, std::forward<Func>(func));
    }

    // NOLINTNEXTLINE(hicpp-explicit-conversions)
    [[nodiscard]] operator Id() const noexcept { return _id; }

private:
    Registry<Id>* _reg;
    Id _id;
};

}  // namespace snir
