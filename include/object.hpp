/**
 * @brief 对象基类
 * @author Ricky
 * @date 2024/11/24
 * @version 2.0
 */
#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "cstring.hpp"

namespace my {

/**
 * @class Object
 * @brief CRTP 对象基类
 * @details 提供哈希、比较、相等、字符串表示的统一接口
 * @tparam D 派生类型
 */
template <typename D>
class Object {
public:
    using derived_obj = D;

    /**
     * @brief 获取对象哈希值
     */
    [[nodiscard]] auto hash() const -> hash_t {
        return my_hash(reinterpret_cast<std::uintptr_t>(this));
    }

    /**
     * @brief 比较两个对象
     */
    [[nodiscard]] auto cmp(const D& other) const -> cmp_t {
        return static_cast<cmp_t>(reinterpret_cast<std::uintptr_t>(this) - reinterpret_cast<std::uintptr_t>(&other));
    }

    /**
     * @brief 判断两个对象是否相等
     */
    [[nodiscard]] auto eq(const D& other) const -> bool {
        return static_cast<const D*>(this)->cmp(other) == 0;
    }

    /**
     * @brief 获取对象字符串表示
     */
    [[nodiscard]] auto to_string() const -> CString {
        std::string type_name = dtype(D);
        i32 len = static_cast<i32>(type_name.size() + 32);
        CString s(len);
#ifdef _MSC_VER
        sprintf_s(s.data(), len, "<%s 0x%p>", type_name.c_str(), static_cast<const void*>(this));
#else
        std::sprintf(s.data(), "<%s %p>", type_name.c_str(), static_cast<const void*>(this));
#endif
        return s;
    }
};

/**
 * @brief CRTP 对象约束
 */
template <typename T>
concept MyObject = requires(T& t) {
    typename T::derived_obj;
} && is_instance<T, Object<typename T::derived_obj>>;

/**
 * @brief 对象输出
 */
template <MyObject T>
auto operator<<(std::ostream& out, const T& obj) -> std::ostream& {
    out << std::format("{}", obj.to_string());
    return out;
}

/**
 * @brief 非可打印类型输出
 */
template <typename T>
    requires(Not<Printable<T>>)
auto operator<<(std::ostream& out, const T& obj) -> std::ostream& {
    out << '<' << dtype(T) << " 0x" << std::hex << &obj << '>';
    return out;
}

template <MyObject T>
auto operator==(const T& a, const T& b) -> bool {
    return a.eq(b);
}

template <MyObject T>
auto operator!=(const T& a, const T& b) -> bool {
    return !a.eq(b);
}

template <MyObject T>
auto operator>(const T& a, const T& b) -> bool {
    return a.cmp(b) > 0;
}

template <MyObject T>
auto operator<(const T& a, const T& b) -> bool {
    return a.cmp(b) < 0;
}

template <MyObject T>
auto operator>=(const T& a, const T& b) -> bool {
    return a.cmp(b) >= 0;
}

template <MyObject T>
auto operator<=(const T& a, const T& b) -> bool {
    return a.cmp(b) <= 0;
}

} // namespace my

#endif // OBJECT_HPP

