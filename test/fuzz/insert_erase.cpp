#include <ankerl/unordered_dense.h>
#include <fuzz/provider.h>

#if defined(FUZZ)
#    define REQUIRE(x) ::fuzz::provider::require(x) // NOLINT(cppcoreguidelines-macro-usage)
#else
#    include <doctest.h>
#endif

#include <cstddef>       // for size_t
#include <cstdint>       // for uint64_t, uint8_t
#include <deque>         // for deque
#include <functional>    // for equal_to
#include <iterator>      // for __iter_val_t, __iter_key_t
#include <unordered_map> // for unordered_map, operator==, hash
#include <vector>        // for allocator, vector

namespace {

// Using DummyHash to make it easier for the fuzzer
struct dummy_hash {
    using is_avalanching = void;

    auto operator()(uint64_t x) const noexcept -> size_t {
        return static_cast<size_t>(x);
    }
};

template <typename Map>
void insert_erase(uint8_t const* data, size_t size) {
    auto p = fuzz::provider(data, size);

    auto ank = Map();
    auto ref = std::unordered_map<uint64_t, uint64_t>();

    auto c = uint64_t();
    while (p.has_remaining_bytes()) {
        auto key = p.integral<uint64_t>();
        ank[key] = c;
        ref[key] = c;
        ++c;

        key = p.integral<uint64_t>();
        REQUIRE(ank.erase(key) == ref.erase(key));
        REQUIRE(ank.size() == ref.size());
    }
    auto cpy = std::unordered_map(ank.begin(), ank.end());
    REQUIRE(cpy == ref);
}

} // namespace

namespace fuzz {

void insert_erase_map(uint8_t const* data, size_t size) {
    insert_erase<ankerl::unordered_dense::map<uint64_t, uint64_t, dummy_hash>>(data, size);
}
void insert_erase_segmented_map(uint8_t const* data, size_t size) {
    insert_erase<ankerl::unordered_dense::segmented_map<uint64_t, uint64_t, dummy_hash>>(data, size);
}
void insert_erase_deque_map(uint8_t const* data, size_t size) {
    insert_erase<ankerl::unordered_dense::map<uint64_t,
                                              uint64_t,
                                              ankerl::unordered_dense::hash<uint64_t>,
                                              std::equal_to<uint64_t>,
                                              std::deque<std::pair<uint64_t, uint64_t>>>>(data, size);
}
} // namespace fuzz

#if defined(FUZZ)
// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" auto LLVMFuzzerTestOneInput(uint8_t const* data, size_t size) -> int {
    fuzz::insert_erase_map(data, size);
    return 0;
}
#endif