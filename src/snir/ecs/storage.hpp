#pragma once

#include "snir/core/print.hpp"

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
    DenseStorage()  = default;
    ~DenseStorage() = default;

    [[nodiscard]] auto get(unsigned index) -> decltype(auto)
    {
        assert(index < vec.size());
        return vec[index];
    }

    template<typename... Args>
    auto emplace(unsigned index, Args&&... args) -> decltype(auto)
    {
        println("Dense::emplace");
        if (vec.size() <= index) {
            vec.resize(index + 1);
        }

        auto& comp = vec[index];
        comp       = Component(std::forward<Args>(args)...);
        return comp;
    }

    template<typename... Func>
    auto patch(unsigned index, Func&&... func) -> decltype(auto)
    {
        assert(index < vec.size());
        auto& comp = vec[index];
        (std::forward<Func>(func)(comp), ...);
        return comp;
    }

private:
    std::vector<Component> vec{};
};

template<typename Component>
struct SparseStorage final : StorageBase
{
    SparseStorage()  = default;
    ~SparseStorage() = default;

    [[nodiscard]] auto get(unsigned index) -> decltype(auto)
    {
        assert(_map.contains(index));
        return _map.at(index);
    }

    template<typename... Args>
    auto emplace(unsigned index, Args&&... args) -> decltype(auto)
    {
        println("Sparse::emplace");
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
