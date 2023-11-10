#pragma once

#include "snir/ecs/storage.hpp"

#include <tuple>
#include <utility>

namespace snir {

template<typename Id, typename... Comps>
struct View
{
    explicit View(std::tuple<StorageTypeFor<Id, Comps>*...> storage) : _storage{std::move(storage)} {}

    [[nodiscard]] auto get(Id id) -> std::tuple<Comps&...>
    {
        return {std::get<StorageTypeFor<Id, Comps>*>(_storage)->get(unsigned(id))...};
    }

private:
    std::tuple<StorageTypeFor<Id, Comps>*...> _storage;
};

}  // namespace snir
