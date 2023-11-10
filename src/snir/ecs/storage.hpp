#pragma once

#include <cassert>
#include <map>
#include <optional>
#include <vector>

namespace snir {

struct StorageBase
{
    StorageBase()          = default;
    virtual ~StorageBase() = default;
};

template<typename Component>
struct DenseStorage final : StorageBase
{
    DenseStorage()           = default;
    ~DenseStorage() override = default;

    [[nodiscard]] auto get(unsigned index) -> decltype(auto)
    {
        assert(index < _vec.size());
        return _vec[index];
    }

    template<typename... Args>
    auto emplace(unsigned index, Args&&... args) -> decltype(auto)
    {
        if (_vec.size() <= index) {
            _vec.resize(index + 1);
        }

        auto& comp = _vec[index];
        comp       = Component(std::forward<Args>(args)...);
        return comp;
    }

    template<typename... Func>
    auto patch(unsigned index, Func&&... func) -> decltype(auto)
    {
        assert(index < _vec.size());
        auto& comp = _vec[index];
        (std::forward<Func>(func)(comp), ...);
        return comp;
    }

private:
    std::vector<Component> _vec{};
};

template<typename Component>
struct SparseStorage final : StorageBase
{
    SparseStorage()           = default;
    ~SparseStorage() override = default;

    [[nodiscard]] auto get(unsigned index) -> decltype(auto)
    {
        assert(_map.contains(index));
        return _map.at(index);
    }

    template<typename... Args>
    auto emplace(unsigned index, Args&&... args) -> decltype(auto)
    {
        return _map.emplace(index, std::forward<Args>(args)...).first->second;
    }

    template<typename... Func>
    auto patch(unsigned index, Func&&... func) -> decltype(auto)
    {
        assert(_map.contains(index));

        auto& comp = get(index);
        (std::forward<Func>(func)(comp), ...);
        return comp;
    }

private:
    std::map<unsigned, Component> _map{};
};

template<typename Id, typename T>
struct StorageTraits
{
    using IdType        = Id;
    using ComponentType = T;
    using StorageType   = DenseStorage<T>;
};

template<typename Id, typename T>
using StorageTypeFor = typename StorageTraits<Id, T>::StorageType;

}  // namespace snir
