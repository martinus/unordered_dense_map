#include <ankerl/unordered_dense.h>

#include <doctest.h>

#include <cstddef> // for size_t
#include <memory>  // for operator!=, unique_ptr, make_unique
#include <utility> // for move, pair
#include <vector>  // for vector

TEST_CASE("unique_ptr") {
    using Map = ankerl::unordered_dense::map<size_t, std::unique_ptr<int>>;
    Map m;
    REQUIRE(m.end() == m.find(123));
    REQUIRE(m.end() == m.begin());
    m[static_cast<size_t>(32)] = std::make_unique<int>(123);
    REQUIRE(m.end() != m.begin());
    REQUIRE(m.end() == m.find(123));
    REQUIRE(m.end() != m.find(32));

    for (auto const& kv : m) {
        REQUIRE(kv.first == 32);
        REQUIRE(kv.second != nullptr);
        REQUIRE(*kv.second == 123);
    }

    m = Map();
    REQUIRE(m.end() == m.begin());
    REQUIRE(m.end() == m.find(123));
    REQUIRE(m.end() == m.find(32));

    Map mEmpty;
    Map m3(std::move(mEmpty));
    REQUIRE(m3.end() == m3.begin());
    REQUIRE(m3.end() == m3.find(123));
    REQUIRE(m3.end() == m3.find(32));
    m3[static_cast<size_t>(32)];
    REQUIRE(m3.end() != m3.begin());
    REQUIRE(m3.end() == m3.find(123));
    REQUIRE(m3.end() != m3.find(32));

    mEmpty = Map{};
    Map m4(std::move(mEmpty));
    REQUIRE(m4.count(123) == 0);
    REQUIRE(m4.end() == m4.begin());
    REQUIRE(m4.end() == m4.find(123));
    REQUIRE(m4.end() == m4.find(32));
}

TEST_CASE("unique_ptr_fill") {
    using Map = ankerl::unordered_dense::map<size_t, std::unique_ptr<int>>;

    Map m;
    for (int i = 0; i < 1; ++i) {
        // m.emplace(i % 500, std::make_unique<int>(i));
        // m.emplace(static_cast<size_t>(123), new int(i));
    }
}
