/**
 * @brief 项目基本类型定义
 * @author Ricky
 * @date 2025/12/16
 * @version 1.0
 */
#ifndef MY_TYPES_HPP
#define MY_TYPES_HPP

#include <cstdint>
#include <cfloat>
#include <limits>

namespace my {

// 有符号整数类型
using i8 = ::int8_t;
using i16 = ::int16_t;
using i32 = ::int32_t;
using i64 = ::int64_t;

// 无符号整数类型
using u8 = ::uint8_t;
using u16 = ::uint16_t;
using u32 = ::uint32_t;
using u64 = ::uint64_t;

// 架构相关类型（指针大小）
using isize = ::intptr_t;
using usize = ::uintptr_t;

// 浮点类型
using f32 = float;
using f64 = double;
using f128 = long double; // 暂时不支持最值和字面量

using cmp_t = i64;  // compare type
using hash_t = u64; // hash type

/**
 * @brief 类型范围
 */
inline constexpr i8 I8_MIN = INT8_MIN;
inline constexpr i8 I8_MAX = INT8_MAX;
inline constexpr i16 I16_MIN = INT16_MIN;
inline constexpr i16 I16_MAX = INT16_MAX;
inline constexpr i32 I32_MIN = INT32_MIN;
inline constexpr i32 I32_MAX = INT32_MAX;
inline constexpr i64 I64_MIN = INT64_MIN;
inline constexpr i64 I64_MAX = INT64_MAX;

inline constexpr u8 U8_MIN = 0;
inline constexpr u8 U8_MAX = UINT8_MAX;
inline constexpr u16 U16_MIN = 0;
inline constexpr u16 U16_MAX = UINT16_MAX;
inline constexpr u32 U32_MIN = 0;
inline constexpr u32 U32_MAX = UINT32_MAX;
inline constexpr u64 U64_MIN = 0;
inline constexpr u64 U64_MAX = UINT64_MAX;

inline constexpr f32 F32_MIN = FLT_MIN; // 最小正正规化数
inline constexpr f32 F32_MAX = FLT_MAX; // 最大正有限数
inline constexpr f64 F64_MIN = DBL_MIN; // 最小正正规化数
inline constexpr f64 F64_MAX = DBL_MAX; // 最大正有限数

#define TYPE_MAX(T) STD numeric_limits<T>().max()
#define TYPE_MIN(T) STD numeric_limits<T>().min()

// 整数字面量
[[nodiscard]] consteval auto operator""_i8(unsigned long long value) -> i8 {
    constexpr auto max = std::numeric_limits<i8>::max();
    if (value > static_cast<unsigned long long>(max)) {
        throw "value too large for i8";
    }
    return static_cast<i8>(value);
}

[[nodiscard]] consteval auto operator""_i16(unsigned long long value) -> i16 {
    constexpr auto max = std::numeric_limits<i16>::max();
    if (value > static_cast<unsigned long long>(max)) {
        throw "value too large for i16";
    }
    return static_cast<i16>(value);
}

[[nodiscard]] consteval auto operator""_i32(unsigned long long value) -> i32 {
    constexpr auto max = std::numeric_limits<i32>::max();
    if (value > static_cast<unsigned long long>(max)) {
        throw "value too large for i32";
    }
    return static_cast<i32>(value);
}

[[nodiscard]] consteval auto operator""_i64(unsigned long long value) -> i64 {
    return static_cast<i64>(value);
}

// 无符号整数字面量
[[nodiscard]] consteval auto operator""_u8(unsigned long long value) -> u8 {
    constexpr auto max = std::numeric_limits<u8>::max();
    if (value > max) {
        throw "value too large for u8";
    }
    return static_cast<u8>(value);
}

[[nodiscard]] consteval auto operator""_u16(unsigned long long value) -> u16 {
    constexpr auto max = std::numeric_limits<u16>::max();
    if (value > max) {
        throw "value too large for u16";
    }
    return static_cast<u16>(value);
}

[[nodiscard]] consteval auto operator""_u32(unsigned long long value) -> u32 {
    constexpr auto max = std::numeric_limits<u32>::max();
    if (value > max) {
        throw "value too large for u32";
    }
    return static_cast<u32>(value);
}

[[nodiscard]] consteval auto operator""_u64(unsigned long long value) -> u64 {
    return static_cast<u64>(value);
}

// 浮点数字面量
[[nodiscard]] constexpr auto operator""_f32(long double value) -> f32 {
    return static_cast<f32>(value);
}

[[nodiscard]] constexpr auto operator""_f64(long double value) -> f64 {
    return static_cast<f64>(value);
}

} // namespace my
#endif // MY_TYPES_HPP
