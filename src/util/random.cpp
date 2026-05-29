#include "random.hpp"

namespace my::util {

Random::Random() :
        engine_(std::random_device{}()) {}

Random::Random(const u64 seed) :
        engine_(seed) {}

void Random::seed(const u64 seed) {
    engine_.seed(seed);
}

bool Random::bernoulli(const f64 p) {
    if (p < 0.0 || p > 1.0) {
        throw argument_exception("Random::bernoulli requires p in [0,1], but given p: {}", p);
    }

    std::bernoulli_distribution dist(p);
    return dist(engine_);
}

CString Random::string(const usize len) {
    static constexpr auto charset =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    constexpr usize charset_size = 62;

    CString result(len);
    std::uniform_int_distribution<usize> dist(0, charset_size - 1);
    for (usize i = 0; i < len; ++i) {
        result[i] = charset[dist(engine_)];
    }
    return result;
}

util::Vec<i32> Random::partition_nonnegative(const i32 n, const i32 sum) {
    util::Vec<i32> result;

    if (n <= 0 || sum < 0) {
        return result;
    }

    /*
     * 特殊情况
     */
    if (n == 1) {
        result.push(sum);
        return result;
    }

    /*
     * Stars and Bars:
     *
     * sum个球
     * n-1个隔板
     *
     * 总长度：
     * sum+n-1
     */
    const i32 total = sum + n - 1;

    /*
     * 生成：
     * [0,total)
     */
    util::Vec<i32> positions;

    positions.reserve(total);

    for (i32 i = 0; i < total; ++i) {
        positions.push(i);
    }

    /*
     * 随机打乱
     */
    shuffle(
        positions.begin(),
        positions.end());

    /*
     * 取前n-1个隔板
     */
    util::Vec<i32> bars;

    bars.reserve(n - 1);

    for (i32 i = 0; i < n - 1; ++i) {
        bars.push(positions[i]);
    }

    std::sort(
        bars.begin(),
        bars.end());

    /*
     * 添加边界
     */
    bars.insert(0, -1);

    bars.push(total);

    /*
     * 差值减1
     */
    usize limit = bars.len();
    for (usize i = 1; i < limit; ++i) {
        result.push(bars[i] - bars[i - 1] - 1);
    }

    return result;
}

Random& Random::thread_local_rng() {
    thread_local Random rng(std::random_device{}());
    return rng;
}

} // namespace my::util