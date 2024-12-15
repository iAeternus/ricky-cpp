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
constexpr int64_t exp2[] = {
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
};

} // namespace my::util

#endif // BINARY_UTILS_HPP