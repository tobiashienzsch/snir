#pragma once

#include <utility>
#include <vector>

namespace snir {

template<typename Id, typename Value>
struct ValueStore
{
    ValueStore() = default;

    [[nodiscard]] auto add(Value value) -> Id
    {
        auto id = Id(_values.size());
        _values.push_back(std::move(value));
        return id;
    }

    [[nodiscard]] auto addDefaultValue() -> Id { return add(Value{}); }

    [[nodiscard]] auto get(Id id) -> Value& { return _values.at(static_cast<std::size_t>(id)); }

    [[nodiscard]] auto get(Id id) const -> Value const&
    {
        return _values.at(static_cast<std::size_t>(id));
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t { return _values.size(); }

    auto reserve(std::size_t capacity) -> void { _values.reserve(capacity); }

private:
    std::vector<Value> _values;
};

}  // namespace snir
