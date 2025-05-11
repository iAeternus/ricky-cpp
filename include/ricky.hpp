/**
 * @brief 基本工具与定义，不依赖任何自定义头文件
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef RICKY_HPP
#define RICKY_HPP

#include <cstdint>
#include <cassert>
#include <typeinfo>
#include <limits>

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
#define neg_index(index, size) (((index) == (size)) ? (index) : (((index) + (size)) % (size)))

#define ifelse(expr, t, f) ((expr) ? (t) : (f))

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
#define I8_MIN std::numeric_limits<i8>::min()
#define I8_MAX std::numeric_limits<i8>::max()
#define I16_MIN std::numeric_limits<i16>::min()
#define I16_MAX std::numeric_limits<i16>::max()
#define I32_MIN std::numeric_limits<i32>::min()
#define I32_MAX std::numeric_limits<i32>::max()
#define I64_MIN std::numeric_limits<i64>::min()
#define I64_MAX std::numeric_limits<i64>::max()

#define U8_MIN std::numeric_limits<u8>::min()
#define U8_MAX std::numeric_limits<u8>::max()
#define U16_MIN std::numeric_limits<u16>::min()
#define U16_MAX std::numeric_limits<u16>::max()
#define U32_MIN std::numeric_limits<u32>::min()
#define U32_MAX std::numeric_limits<u32>::max()
#define U64_MIN std::numeric_limits<u64>::min()
#define U64_MAX std::numeric_limits<u64>::max()

#define F32_MIN std::numeric_limits<f32>::min()
#define F32_MAX std::numeric_limits<f32>::max()
#define F64_MIN std::numeric_limits<f64>::min()
#define F64_MAX std::numeric_limits<f64>::max()

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