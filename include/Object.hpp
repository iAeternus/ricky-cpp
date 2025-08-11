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
 * @class Object
 * @brief 根对象（CRTP基类）
 * @details 提供基本的哈希、比较和字符串表示功能
 * @tparam D 派生类类型
 * @note 使用CRTP模式实现静态多态，避免虚函数的开销
 * @note 该类不应该被直接实例化，而是作为其他类的基类
 * @note 派生类需要实现__hash__()、__cmp__()和__str__()方法
 */
template <typename D>
class Object {
public:
    using derived_obj = D;

    /**
     * @brief 获取对象的哈希值
     */
    [[nodiscard]] hash_t __hash__() const {
        static_assert(false, "NotImplementedException: not implemented __hash__()");
        std::unreachable();
    }

    /**
     * @brief 比较两个对象
     * @param other 另一个对象
     * @return 返回值大于0为大于，小于0为小于，等于0为等于
     */
    [[nodiscard]] cmp_t __cmp__(const D& other) const {
        return static_cast<cmp_t>(this) - static_cast<cmp_t>(&other);
    }

    /**
     * @brief 判断两个对象是否相等
     * @details 如果两个对象的哈希值相等且比较结果为0，则认为它们相等
     * @note 该方法不应该抛出异常，若发生异常则返回false
     * @note 该方法不应该修改对象状态
     * @param other 另一个对象
     * @return true=相等 false=不相等
     */
    [[nodiscard]] bool __equals__(const D& other) const {
        return static_cast<const D*>(this)->__cmp__(other) == 0;
    }

    /**
     * @brief 获取对象的字符串表示
     * @note 该函数不应该抛出异常，若发生异常则返回一个空字符串
     * @note 该函数不应该修改对象状态
     * @return 对象的字符串表示
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

/**
 * @brief 输出流操作符重载
 * @details 支持MyObject类型的对象输出
 * @tparam T 对象类型
 * @param out 输出流
 * @param obj 对象
 * @return 输出流
 */
template <MyObject T>
std::ostream& operator<<(std::ostream& out, const T& obj) {
    out << obj.__str__().data();
    return out;
}

/**
 * @brief 输出流操作符重载
 * @details 对于不支持__str__()的类型，输出类型名和地址
 * @tparam T 对象类型
 * @param out 输出流
 * @param obj 对象
 * @return 输出流
 */
template <typename T>
    requires(Not<Printable<T>>)
std::ostream& operator<<(std::ostream& out, const T& obj) {
    out << '<' << dtype(T) << " 0x" << std::hex << &obj << '>';
    return out;
}

/**
 * @brief 比较操作符重载
 * @details 支持MyObject类型的对象比较
 * @tparam T 对象类型
 * @param a 左侧对象
 * @param b 右侧对象
 * @return 比较结果
 */
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