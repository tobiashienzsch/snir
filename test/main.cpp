#include "snir/core/static_vector.hpp"
#include "snir/core/strings.hpp"

#undef NDEBUG
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

namespace {

auto testVector() -> void  // NOLINT(readability-function-cognitive-complexity)
{
    auto test = []<typename T>(T val) {  // NOLINT(readability-function-cognitive-complexity)
        using Vec = snir::StaticVector<T, 2>;
        static_assert(std::same_as<typename Vec::value_type, T>);
        static_assert(std::same_as<typename Vec::value_type, T>);

        auto vec = Vec{};
        assert(vec.empty());
        assert(not vec.full());
        assert(vec.capacity() == 2);
        assert(vec.size() == 0);  // NOLINT

        vec.push_back(val);
        assert(not vec.empty());
        assert(not vec.full());
        assert(vec.size() == 1);
        assert(std::distance(vec.begin(), vec.end()) == 1);
        assert(std::distance(std::ranges::begin(vec), std::ranges::end(vec)) == 1);
        assert(std::distance(std::as_const(vec).begin(), std::as_const(vec).end()) == 1);
        assert(vec[0] == val);

        vec.push_back(static_cast<T>(val + val));
        assert(not vec.empty());
        assert(vec.size() == 2);
        assert(vec.full());
        assert(std::distance(vec.begin(), vec.end()) == 2);
        assert(std::distance(std::ranges::begin(vec), std::ranges::end(vec)) == 2);
        assert(std::distance(std::as_const(vec).begin(), std::as_const(vec).end()) == 2);
        assert(vec[0] == val);
        assert(vec[1] == val + val);
        assert(std::as_const(vec)[0] == val);
        assert(std::as_const(vec)[1] == val + val);

        auto const other = vec;
        assert(std::ranges::equal(vec, other));

        auto const list = Vec{val, val};
        assert(list.size() == 2);
        assert(list[0] == val);
        assert(list[1] == val);

        try {
            [[maybe_unused]] auto tooBig = Vec{val, val, val};
            assert(false);
        } catch (const std::exception& e) {
            assert(snir::strings::contains(e.what(), "initializer_list out-of-bounds size: 3"));
        }

        try {
            [[maybe_unused]] auto newVal = vec[42];
            assert(false);
        } catch (const std::exception& e) {
            assert(snir::strings::contains(e.what(), "subscript out-of-bounds idx: 42, size: 2"));
        }

        try {
            [[maybe_unused]] auto newVal = vec.push_back(val);
            assert(false);
        } catch (const std::exception& e) {
            assert(snir::strings::contains(e.what(), "push_back on full StaticVector<T, 2>"));
        }
    };

    test(std::uint8_t{42});
    test(std::uint16_t{42});
    test(std::uint32_t{42});
    test(std::uint64_t{42});
    test(std::int8_t{42});
    test(std::int16_t{42});
    test(std::int32_t{42});
    test(std::int64_t{42});
    test(float{42});
    test(double{42});

    static_assert(sizeof(snir::StaticVector<std::uint8_t, 1>{}) == 2);
    static_assert(sizeof(snir::StaticVector<std::uint8_t, 2>{}) == 3);
    static_assert(sizeof(snir::StaticVector<std::uint8_t, 3>{}) == 4);

    static_assert(sizeof(snir::StaticVector<std::uint16_t, 1>{}) == 4);
    static_assert(sizeof(snir::StaticVector<std::uint16_t, 2>{}) == 6);
    static_assert(sizeof(snir::StaticVector<std::uint16_t, 3>{}) == 8);

    static_assert(sizeof(snir::StaticVector<std::uint32_t, 1>{}) == 8);
    static_assert(sizeof(snir::StaticVector<std::uint32_t, 2>{}) == 12);
    static_assert(sizeof(snir::StaticVector<std::uint32_t, 3>{}) == 16);
}

}  // namespace

auto main() -> int
{
    testVector();
    return 0;
}
