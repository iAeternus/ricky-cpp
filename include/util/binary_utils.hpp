/**
 * @brief 二进制工具
 * @author Ricky
 * @date 2024/12/2
 * @version 1.0
 */
#ifndef BINARY_UTILS_HPP
#define BINARY_UTILS_HPP

#include "ricky.hpp"

namespace my::util {

/**
 * @brief exp2[i]: 2^i
 */
constexpr i64 exp2[] = {
    1ll << 0,
    1ll << 1,
    1ll << 2,
    1ll << 3,
    1ll << 4,
    1ll << 5,
    1ll << 6,
    1ll << 7,
    1ll << 8,
    1ll << 9,
    1ll << 10,
    1ll << 11,
    1ll << 12,
    1ll << 13,
    1ll << 14,
    1ll << 15,
    1ll << 16,
    1ll << 17,
    1ll << 18,
    1ll << 19,
    1ll << 20,
    1ll << 21,
    1ll << 22,
    1ll << 23,
    1ll << 24,
    1ll << 25,
    1ll << 26,
    1ll << 27,
    1ll << 28,
    1ll << 29,
    1ll << 30,
    1ll << 31,
    1ll << 32,
    1ll << 33,
    1ll << 34,
    1ll << 35,
    1ll << 36,
    1ll << 37,
    1ll << 38,
    1ll << 39,
    1ll << 40,
    1ll << 41,
    1ll << 42,
    1ll << 43,
    1ll << 44,
    1ll << 45,
    1ll << 46,
    1ll << 47,
    1ll << 48,
    1ll << 49,
    1ll << 50,
    1ll << 51,
    1ll << 52,
    1ll << 53,
    1ll << 54,
    1ll << 55,
    1ll << 56,
    1ll << 57,
    1ll << 58,
    1ll << 59,
    1ll << 60,
    1ll << 61,
    1ll << 62,
    1ll << 63,
};

/**
 * @brief 计算x的二进制表示中由最低位的1及其后面的所有的0的二进制构成的数值
 */
template <typename B>
constexpr fn lowbit(const B& x)->B {
    return x & -x;
}

/**
 * @brief 计算x的二进制表示中最低位的1的索引，索引从0开始
 */
template <typename B>
constexpr fn lowbit_idx(const B& x)->B {
    B l{};
    while ((x >> l & 1) == 0) {
        ++l;
    }
    return l;
}

/**
 * @brief 计算x的二进制表示中最高位的1的索引，索引从0开始
 */
template <typename B>
constexpr fn highbit_idx(const B& x)->B {
    B l{};
    while (x >> l) {
        ++l;
    }
    return --l;
}

/**
 * @brief 计算x的二进制表示中由最高位的1及其后面的所有的位的二进制构成的数值
 */
constexpr fn highbit(u64 x)->u64 {
    x |= (x >> 1ULL);
    x |= (x >> 2ULL);
    x |= (x >> 4ULL);
    x |= (x >> 8ULL);
    x |= (x >> 16ULL);
    x |= (x >> 32ULL);
    return x - (x >> 1ULL);
}

constexpr fn highbit(u32 x)->u32 {
    x |= (x >> 1U);
    x |= (x >> 2U);
    x |= (x >> 4U);
    x |= (x >> 8U);
    x |= (x >> 16U);
    return x - (x >> 1U);
}

/**
 * @brief 判断x的二进制表示是否全为1
 * @return true=是 false=否
 */
template <typename B>
constexpr fn all_one(const B& x)->bool {
    return (x & x + 1) == 0;
}

/**
 * @brief 判断x的二进制表示是否只有一个1
 * @return true=是 false=否
 */
template <typename B>
constexpr fn only_one(const B& x)->bool {
    return (x & x - 1) == 0;
}

/**
 * @brief 向上取整到2的幂
 */
constexpr fn roundup2(usize x)->usize {
    if (x <= 1) return 0LL;

    --x;
    for (usize i = 1LL; i < sizeof(usize) * 8LL; i <<= 1LL) {
        x |= x >> 1LL;
    }
    return x + 1LL;
}

} // namespace my::util

#endif // BINARY_UTILS_HPP