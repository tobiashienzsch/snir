#pragma once

#include "snir/ecs/storage.hpp"

#include <tuple>
#include <utility>

namespace snir {

template<typename Id, typename... Comps>
struct View
{
    explicit View(std::tuple<StorageTypeFor<Id, Comps>*...> storage) : _storage{std::move(storage)} {}

    [[nodiscard]] auto get(Id id) -> decltype(auto)
        requires(sizeof...(Comps) == 1)
    {
        return std::get<0>(_storage)->get(unsigned(id));
    }

    [[nodiscard]] auto get(Id id) -> std::tuple<Comps&...>
        requires(sizeof...(Comps) > 1)
    {
        return {std::get<StorageTypeFor<Id, Comps>*>(_storage)->get(unsigned(id))...};
    }

private:
    std::tuple<StorageTypeFor<Id, Comps>*...> _storage;
};

}  // namespace snir
