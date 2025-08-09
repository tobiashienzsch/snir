#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace snir {

namespace detail {

template<typename T, typename = void>
inline constexpr bool isTransparent = false;

template<typename T>
inline constexpr bool isTransparent<T, std::void_t<typename T::is_transparent>> = true;

}  // namespace detail

struct SortedUniqueTag
{
    explicit SortedUniqueTag() = default;
};

inline constexpr SortedUniqueTag SortedUnique{};

template<typename Key, typename Compare = std::less<Key>, typename Container = std::vector<Key>>
struct FlatSet
{
    using key_type               = Key;
    using key_compare            = Compare;
    using value_type             = Key;
    using value_compare          = Compare;
    using reference              = value_type&;
    using const_reference        = value_type const&;
    using size_type              = typename Container::size_type;
    using difference_type        = typename Container::difference_type;
    using iterator               = typename Container::iterator;
    using const_iterator         = typename Container::const_iterator;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using container_type         = Container;

    constexpr FlatSet() : FlatSet{Compare{}} {}

    /// \brief Initializes c with std::move(cont), value-initializes compare,
    /// sorts the range [begin(),end()) with respect to compare, and finally
    /// erases the range [ranges::unique(*this, compare), end());
    ///
    /// Complexity: Linear in N if cont is sorted with respect to compare and
    /// otherwise N log N, where N is cont.size().
    explicit constexpr FlatSet(container_type const& container)
        : FlatSet{std::ranges::begin(container), std::ranges::end(container), Compare()}
    {}

    constexpr FlatSet(SortedUniqueTag /*tag*/, container_type cont)
        : _container{std::move(cont)}
        , _compare{Compare()}
    {}

    explicit constexpr FlatSet(Compare const& comp) : _container{}, _compare(comp) {}

    template<typename InputIt>
    constexpr FlatSet(InputIt first, InputIt last, Compare const& comp = Compare())
        : _container{}
        , _compare(comp)
    {
        insert(first, last);
    }

    template<typename InputIt>
    constexpr FlatSet(
        SortedUniqueTag /*tag*/,
        InputIt first,
        InputIt last,
        Compare const& comp = Compare()
    )
        : _container(first, last)
        , _compare(comp)
    {}

    [[nodiscard]] constexpr auto begin() noexcept -> iterator { return _container.begin(); }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator
    {
        return _container.begin();
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
    {
        return _container.begin();
    }

    [[nodiscard]] constexpr auto end() noexcept -> iterator { return _container.end(); }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator { return _container.end(); }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator { return _container.end(); }

    [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator { return _container.rbegin(); }

    [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator
    {
        return _container.rbegin();
    }

    [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator
    {
        return _container.crbegin();
    }

    [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator { return _container.rend(); }

    [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator
    {
        return _container.rend();
    }

    [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator
    {
        return _container.crend();
    }

    /// \brief Returns true if the underlying container is empty.
    [[nodiscard]] constexpr auto empty() const noexcept -> bool { return _container.empty(); }

    /// \brief Returns the size of the underlying container.
    [[nodiscard]] constexpr auto size() const noexcept -> size_type { return _container.size(); }

    /// \brief Returns the max_size of the underlying container.
    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type
    {
        return _container.max_size();
    }

    // 21.6.5.3, modifiers
    template<typename... Args>
    constexpr auto emplace(Args&&... args) -> std::pair<iterator, bool>
    {
        auto key    = Key{std::forward<Args>(args)...};
        iterator it = lower_bound(key);

        if (it == end() or _compare(key, *it)) {
            it = _container.emplace(it, std::move(key));
            return {it, true};
        }

        return {it, false};
    }

    template<typename... Args>
    constexpr auto emplace_hint(const_iterator /*position*/, Args&&... args) -> iterator
    {
        return emplace(std::forward<Args>(args)...).first;
    }

    constexpr auto insert(value_type const& x) -> std::pair<iterator, bool> { return emplace(x); }

    constexpr auto insert(value_type&& x) -> std::pair<iterator, bool>
    {
        return emplace(std::move(x));
    }

    constexpr auto insert(const_iterator position, value_type const& x) -> iterator
    {
        return emplace_hint(position, x);
    }

    constexpr auto insert(const_iterator position, value_type&& x) -> iterator
    {
        return emplace_hint(position, std::move(x));
    }

    template<typename InputIt>
    constexpr auto insert(InputIt first, InputIt last) -> void
    {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    template<typename InputIt>
    constexpr auto insert(SortedUniqueTag /*tag*/, InputIt first, InputIt last) -> void;

    constexpr auto extract() && -> container_type
    {
        auto&& container = std::move(_container);
        clear();
        return container;
    }

    constexpr auto replace(container_type&& container) -> void { _container = std::move(container); }

    constexpr auto erase(iterator position) -> iterator { return _container.erase(position); }

    constexpr auto erase(const_iterator position) -> iterator { return _container.erase(position); }

    constexpr auto erase(key_type const& key) -> size_type
    {
        auto const it = std::remove(begin(), end(), key);
        auto const r  = static_cast<size_type>(std::distance(it, end()));
        erase(it, end());
        return r;
    }

    constexpr auto erase(const_iterator first, const_iterator last) -> iterator
    {
        return _container.erase(first, last);
    }

    constexpr auto swap(
        FlatSet& other
    ) noexcept(std::is_nothrow_swappable_v<Container> && std::is_nothrow_swappable_v<Compare>) -> void
    {
        using std::swap;
        swap(_compare, other._compare);
        swap(_container, other._container);
    }

    constexpr auto clear() noexcept -> void { _container.clear(); }

    // observers
    [[nodiscard]] constexpr auto key_comp() const -> key_compare { return _compare; }

    [[nodiscard]] constexpr auto value_comp() const -> value_compare { return _compare; }

    // set operations
    [[nodiscard]] constexpr auto find(key_type const& key) -> iterator
    {
        auto const it = lower_bound(key);
        if (it == end() or _compare(key, *it)) {
            return end();
        }
        return it;
    }

    [[nodiscard]] constexpr auto find(key_type const& key) const -> const_iterator
    {
        auto const it = lower_bound(key);
        if (it == end() or _compare(key, *it)) {
            return end();
        }
        return it;
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto find(K const& key) -> iterator
    {
        auto const it = lower_bound(key);
        if (it == end() or _compare(key, *it)) {
            return end();
        }
        return it;
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto find(K const& key) const -> const_iterator
    {
        auto const it = lower_bound(key);
        if (it == end() or _compare(key, *it)) {
            return end();
        }
        return it;
    }

    [[nodiscard]] constexpr auto count(key_type const& key) const -> size_type
    {
        return find(key) == end() ? 0 : 1;
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto count(K const& key) const -> size_type
    {
        return find(key) == end() ? 0 : 1;
    }

    [[nodiscard]] constexpr auto contains(key_type const& key) const -> bool
    {
        return count(key) == 1;
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto contains(K const& key) const -> bool
    {
        return count(key) == 1;
    }

    [[nodiscard]] constexpr auto lower_bound(key_type const& key) -> iterator
    {
        return std::ranges::lower_bound(*this, key, std::ref(_compare));
    }

    [[nodiscard]] constexpr auto lower_bound(key_type const& key) const -> const_iterator
    {
        return std::ranges::lower_bound(*this, key, std::ref(_compare));
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto lower_bound(K const& key) -> iterator
    {
        return std::ranges::lower_bound(*this, key, std::ref(_compare));
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto lower_bound(K const& key) const -> const_iterator
    {
        return std::ranges::lower_bound(*this, key, std::ref(_compare));
    }

    [[nodiscard]] constexpr auto upper_bound(key_type const& key) -> iterator
    {
        return std::ranges::upper_bound(*this, key, std::ref(_compare));
    }

    [[nodiscard]] constexpr auto upper_bound(key_type const& key) const -> const_iterator
    {
        return std::ranges::upper_bound(*this, key, std::ref(_compare));
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto upper_bound(K const& key) -> iterator
    {
        return std::ranges::upper_bound(*this, key, std::ref(_compare));
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto upper_bound(K const& key) const -> const_iterator
    {
        return std::ranges::upper_bound(*this, key, std::ref(_compare));
    }

    [[nodiscard]] constexpr auto equal_range(key_type const& key) -> std::pair<iterator, iterator>
    {
        return std::ranges::equal_range(*this, key, std::ref(_compare));
    }

    [[nodiscard]] constexpr auto equal_range(key_type const& key) const
        -> std::pair<const_iterator, const_iterator>
    {
        return std::ranges::equal_range(*this, key, std::ref(_compare));
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto equal_range(K const& key) -> std::pair<iterator, iterator>
    {
        return std::ranges::equal_range(*this, key, std::ref(_compare));
    }

    template<typename K>
        requires detail::isTransparent<Compare>
    [[nodiscard]] constexpr auto equal_range(K const& key) const
        -> std::pair<const_iterator, const_iterator>
    {
        return std::ranges::equal_range(*this, key, std::ref(_compare));
    }

    friend constexpr auto operator==(FlatSet const& lhs, FlatSet const& rhs) -> bool
    {
        return std::ranges::equal(lhs, rhs);
    }

    friend constexpr auto operator<(FlatSet const& lhs, FlatSet const& rhs) -> bool
    {
        return std::ranges::lexicographical_compare(lhs, rhs);
    }

    friend constexpr auto operator>(FlatSet const& x, FlatSet const& y) -> bool { return y < x; }

    friend constexpr auto operator<=(FlatSet const& x, FlatSet const& y) -> bool { return !(y < x); }

    friend constexpr auto operator>=(FlatSet const& x, FlatSet const& y) -> bool { return !(x < y); }

    friend constexpr auto swap(FlatSet& x, FlatSet& y) noexcept(noexcept(x.swap(y))) -> void
    {
        return x.swap(y);
    }

private:
    [[no_unique_address]] container_type _container;
    [[no_unique_address]] key_compare _compare;
};

template<typename Key, typename Container, typename Compare, typename Pred>
constexpr auto eraseIf(FlatSet<Key, Container, Compare>& c, Pred pred) ->
    typename FlatSet<Key, Container, Compare>::size_type
{
    auto const it = std::remove_if(c.begin(), c.end(), pred);
    auto const r  = std::distance(it, c.end());
    c.erase(it, c.end());
    return static_cast<typename FlatSet<Key, Container, Compare>::size_type>(r);
}

}  // namespace snir
