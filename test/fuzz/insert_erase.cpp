#include <ankerl/unordered_dense.h>
#include <fuzz/Provider.h>

#if defined(FUZZ)
#    define REQUIRE(x) ::fuzz::Provider::require(x)
#else
#    include <doctest.h>
#endif

#include <unordered_map>

namespace {

// Using DummyHash to make it easier for the fuzzer
struct DummyHash {
    using is_avalanching = void;

    auto operator()(uint64_t x) const noexcept -> size_t {
        return x;
    }
};

} // namespace

namespace fuzz {

void insert_erase(uint8_t const* data, size_t size) {
    auto p = fuzz::Provider(data, size);

    auto ank = ankerl::unordered_dense::map<uint64_t, uint64_t, DummyHash>();
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

} // namespace fuzz

#if defined(FUZZ)
extern "C" auto LLVMFuzzerTestOneInput(uint8_t const* data, size_t size) -> int {
    fuzz::insert_erase(data, size);
    return 0;
}
#endif