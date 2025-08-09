#pragma once

#include "snir/core/Exception.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <type_traits>

namespace snir {

template<typename T, unsigned Capacity>
struct InplaceVector
{
    using value_type             = T;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using pointer                = T*;
    using const_pointer          = T const*;
    using reference              = T&;
    using const_reference        = T const&;
    using iterator               = T*;
    using const_iterator         = T const*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    InplaceVector() = default;

    InplaceVector(std::initializer_list<T> il)
    {
        if (il.size() <= capacity()) {
            std::ranges::copy(il, _buffer.begin());
            _size = static_cast<SizeType>(il.size());
            return;
        }

        raisef<std::out_of_range>("initializer_list out-of-bounds size: {}", il.size());
    }

    [[nodiscard]] auto empty() const noexcept -> bool { return _size == 0; }

    [[nodiscard]] auto full() const noexcept -> bool { return _size == Capacity; }

    [[nodiscard]] auto size() const noexcept -> std::size_t { return _size; }

    [[nodiscard]] auto capacity() const noexcept -> std::size_t
    {
        (void)(this);
        return Capacity;
    }

    [[nodiscard]] auto begin() -> iterator { return _buffer.data(); }

    [[nodiscard]] auto begin() const -> const_iterator { return _buffer.data(); }

    [[nodiscard]] auto end() -> iterator { return std::next(begin(), ptrdiff_t(size())); }

    [[nodiscard]] auto end() const -> const_iterator { return std::next(begin(), ptrdiff_t(size())); }

    [[nodiscard]] auto operator[](std::integral auto index) -> reference
    {
        return subscript(*this, static_cast<size_type>(index));
    }

    [[nodiscard]] auto operator[](std::integral auto index) const -> const_reference
    {
        return subscript(*this, static_cast<size_type>(index));
    }

    template<typename U>
    auto push_back(U&& val) -> reference  // NOLINT(readability-identifier-naming)
    {
        if (full()) {
            raisef<std::out_of_range>("push_back on full InplaceVector<T, {}>", capacity());
        }

        _buffer.at(_size) = std::forward<U>(val);
        ++_size;
        return *std::prev(end());
    }

private:
    template<typename Self>
    [[nodiscard]] static auto subscript(Self&& self, size_type index) -> decltype(auto)
    {
        if (index < self.size()) {
            return *std::next(std::forward<Self>(self).begin(), static_cast<ptrdiff_t>(index));
        }

        raisef<std::out_of_range>("subscript out-of-bounds idx: {}, size: {}", index, self.size());
    }

    static_assert(Capacity <= std::numeric_limits<std::uint16_t>::max());

    using SizeType = std::conditional_t<
        Capacity <= std::numeric_limits<std::uint8_t>::max(),
        std::uint8_t,
        std::uint16_t>;

    std::array<T, Capacity> _buffer{};
    SizeType _size{0};
};

}  // namespace snir
