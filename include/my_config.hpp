/**
 * @brief 基本工具与定义，不依赖任何自定义头文件
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef MY_CONFIG_HPP
#define MY_CONFIG_HPP

#include <utility>

#include "my_types.hpp"

namespace my {

/**
 * @brief rust-like `loop` 关键字
 * @note 规范：
 * 仅可用于无限循环
 */
#define loop while (true)

/**
 * @brief 标准库命名空间
 */
#define STD ::std::

/**
 * @brief 获取类型名
 */
#define dtype(T) typeid(T).name()

/**
 * @brief 负数索引支持
 * @details 取模运算, 运算结果为 [0, size]
 */
#define neg_index(index, size) (((index) == (size)) ? (index) : ((index) + (size)) % (size))

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

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

/**
 * @brief 无效的索引位置
 */
inline constexpr usize npos = static_cast<usize>(-1);

/**
 * 当前代码位置简化宏
 */
#define SRC_LOC std::source_location::current()

} // namespace my

#endif // MY_CONFIG_HPP