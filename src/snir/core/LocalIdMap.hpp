#pragma once

#include "snir/core/Concepts.hpp"

#include <concepts>
#include <cstddef>
#include <map>
#include <vector>

namespace snir {

template<ScopedEnum Key, std::integral Id>
struct LocalIdMap
{
    LocalIdMap() = default;

    [[nodiscard]] auto operator[](Key key) const -> Id { return _ids.at(key); }

    [[nodiscard]] auto operator[](Id id) const -> Key { return _keys.at(static_cast<size_t>(id)); }

    [[nodiscard]] auto add(Key key) -> Id
    {
        auto const [it, inserted] = _ids.emplace(key, static_cast<Id>(_keys.size()));
        if (inserted) {
            _keys.push_back(key);
        }
        return it->second;
    }

    auto clear() -> void
    {
        _ids.clear();
        _keys.clear();
    }

private:
    std::map<Key, Id> _ids;
    std::vector<Key> _keys;
};

}  // namespace snir
