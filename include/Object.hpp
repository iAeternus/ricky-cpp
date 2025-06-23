/**
 * @brief 顶层父类
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "ricky_concepts.hpp"
#include "CString.hpp"

namespace my {

/**
 * @brief 根对象（CRTP基类）
 */
template <typename D>
class Object {
public:
    using derived_obj = D;

    /**
     * 计算hash编码
     */
    [[nodiscard]] hash_t __hash__() const {
        static_assert(false, "NotImplementedException: not implemented __hash__()");
        std::unreachable();
    }

    /**
     * @brief 比较两个对象
     * @return 返回值大于0为大于，小于0为小于，等于0为等于
     */
    [[nodiscard]] cmp_t __cmp__(const D& other) const {
        return static_cast<cmp_t>(this) - static_cast<cmp_t>(&other);
    }

    /**
     * @brief 判断两个对象是否相等
     * @return true=相等 false=不相等
     */
    [[nodiscard]] bool __equals__(const D& other) const {
        return static_cast<const D*>(this)->__cmp__(other) == 0;
    }

    /**
     * @brief 转换为`字符串`类型
     */
    [[nodiscard]] CString __str__() const {
        std::string type_name = dtype(D);
        i32 len = type_name.size() + 22; // TODO ??
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
 * CRTP模式，静态多态约束
 * 相比于使用虚函数的动态多态，开销更小
 */
template <typename T>
concept MyObject = requires(T& t) {
    typename T::derived_obj;
} && is_instance<T, Object<typename T::derived_obj>>;

template <MyObject T>
std::ostream& operator<<(std::ostream& out, const T& obj) {
    out << obj.__str__().data();
    return out;
}

template <typename T> requires(Not<Printable<T>>)
std::ostream& operator<<(std::ostream& out, const T& obj) {
    out << '<' << dtype(T) << " 0x" << std::hex << &obj << '>';
    return out;
}

template <MyObject T>
bool operator==(const T& a, const T& b) { return a.__equals__(b); }

template <MyObject T>
bool operator!=(const T& a, const T& b) { return !a.__equals__(b); }

template <MyObject T>
bool operator>(const T& a, const T& b) { return a.__cmp__(b) > 0; }

template <MyObject T>
bool operator<(const T& a, const T& b) { return a.__cmp__(b) < 0; }

template <MyObject T>
bool operator>=(const T& a, const T& b) { return a.__cmp__(b) >= 0; }

template <MyObject T>
bool operator<=(const T& a, const T& b) { return a.__cmp__(b) <= 0; }

} // namespace my

#endif // OBJECT_HPP