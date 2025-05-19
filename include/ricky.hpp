/**
 * @brief 基本工具与定义，不依赖任何自定义头文件
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef RICKY_HPP
#define RICKY_HPP

#include <cfloat>
#include <cstdint>
#include <utility>

namespace my {

/**
 * @brief rust-like `fn` 关键字
 * @note 规范：
 * 冠以`fn`的函数，应适合内联，且返回值应后置
 * 对于静态函数，`fn`应该放在static后
 * 对于constexpr函数，`fn`应该放在constexpr后
 */
#define fn inline auto

#define fr(R) inline R

// /**
//  * @brief rust-like `let` 关键字
//  * @note 规范：
//  * 仅可用于局部变量创建
//  */
// #define let auto

/**
 * @brief 获取类型名
 */
#define dtype(T) typeid(T).name()

/**
 * @brief 取模运算, 运算结果为 [0, size]
 */
template <typename Index, typename Size>
[[nodiscard]] constexpr auto neg_index(Index index, Size size) noexcept {
    return (index == size) ? index : ((index + size) % size);
}

template <typename T, typename F>
[[nodiscard]] constexpr decltype(auto) ifelse(bool expr, T&& t, F&& f) noexcept(
    noexcept(expr ? std::forward<T>(t) : std::forward<F>(f))) {
    return expr ? std::forward<T>(t) : std::forward<F>(f);
}

#define hastype(T, type)  \
    requires(T t) {       \
        typename T::type; \
    }

#define hasmember(T, member) \
    requires(T t) {          \
        &T::member;          \
    }

#define hasmethod(T, method, ...) \
    requires(T t) {               \
        t.method(##__VA_ARGS__);  \
    }

#define hasattr(T, attr) hasmember(T, attr) || hastype(T, attr)

/**
 * @brief rust-like 类型
 */
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;
using f128 = long double;

#ifdef _WIN64
// container size type
using isize = i64;
using usize = u64;
using cmp_t = i64;  // compare type
using hash_t = u64; // hash type
#else
// container size type
using isize = i32;
using usize = u32;
using cmp_t = i32;  // compare type
using hash_t = u32; // hash type
#endif

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

/**
 * @brief 类型范围
 */
// 有符号整数极值
inline constexpr i8 I8_MIN = INT8_MIN;
inline constexpr i8 I8_MAX = INT8_MAX;
inline constexpr i16 I16_MIN = INT16_MIN;
inline constexpr i16 I16_MAX = INT16_MAX;
inline constexpr i32 I32_MIN = INT32_MIN;
inline constexpr i32 I32_MAX = INT32_MAX;
inline constexpr i64 I64_MIN = INT64_MIN;
inline constexpr i64 I64_MAX = INT64_MAX;

// 无符号整数极值
inline constexpr u8 U8_MIN = 0;
inline constexpr u8 U8_MAX = UINT8_MAX;
inline constexpr u16 U16_MIN = 0;
inline constexpr u16 U16_MAX = UINT16_MAX;
inline constexpr u32 U32_MIN = 0;
inline constexpr u32 U32_MAX = UINT32_MAX;
inline constexpr u64 U64_MIN = 0;
inline constexpr u64 U64_MAX = UINT64_MAX;

// 浮点极值（IEEE 754）
inline constexpr f32 F32_MIN = FLT_MIN; // 最小正正规化数
inline constexpr f32 F32_MAX = FLT_MAX; // 最大正有限数
inline constexpr f64 F64_MIN = DBL_MIN; // 最小正正规化数
inline constexpr f64 F64_MAX = DBL_MAX; // 最大正有限数

/**
 * @brief 空值, 不会被使用, 仅用于占位符
 */
template <typename T>
T& None = *reinterpret_cast<T*>(NULL);

/**
 * @brief 标准库命名空间
 */
#define STD ::std::

} // namespace my

#endif // RICKY_HPP