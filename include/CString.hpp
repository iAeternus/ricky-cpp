/**
 * @brief 字符串，解决char*不能做返回值的问题
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef CSTRING_HPP
#define CSTRING_HPP

#include "hash.hpp"
#include "ricky_memory.hpp"

#include <cstring>
#include <format>
#include <sstream>

namespace my {

/**
 * @brief 自定义字符串类，提供安全的字符串操作和内存管理
 */
class CString {
    using self = CString;

public:
    /**
     * @brief 默认构造函数，创建一个空字符串
     */
    CString() :
            str_(my_alloc<char>(1)), len_(sizeof(char)) {
        std::memset(str_, 0, 1);
    }

    /**
     * @brief 根据指定长度创建字符串
     * @param len 字符串的长度
     */
    CString(c_size len) :
            str_(my_alloc<char>(len + 1)), len_(len) {
        std::memset(str_, 0, len + 1);
    }

    /**
     * @brief 根据 C 风格字符串创建字符串
     * @param str C 风格字符串
     */
    CString(const char* str) :
            CString(str, std::strlen(str)) {}

    /**
     * @brief 根据 C 风格字符串和指定长度创建字符串
     * @param str C 风格字符串
     * @param len 字符串的长度
     */
    CString(const char* str, c_size len) :
            CString(len) {
        std::memcpy(data(), str, len);
    }

    /**
     * @brief 根据 std::basic_string 创建字符串
     * @param str std::basic_string 对象
     */
    CString(const std::basic_string<char>& str) :
            CString(str.c_str(), str.size()) {}

    /**
     * @brief 析构函数，释放动态分配的内存
     */
    ~CString() {
        my_delloc(str_);
    }

    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 CString 对象
     */
    CString(const self& other) :
            CString(other.data(), other.size()) {}

    /**
     * @brief 移动构造函数
     * @param other 要移动的 CString 对象
     */
    CString(self&& other) noexcept :
            str_(other.str_), len_(other.len_) {
        other.str_ = nullptr;
        other.len_ = 0;
    }

    /**
     * @brief 拷贝赋值操作符
     * @param other 要赋值的 CString 对象
     * @return 自身的引用
     */
    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_delloc(str_);
        return *my_construct(this, other);
    }

    /**
     * @brief 移动赋值操作符
     * @param other 要赋值的 CString 对象
     * @return 自身的引用
     */
    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        my_delloc(str_);
        return *my_construct(this, std::forward<self>(other));
    }

    /**
     * @brief 隐式转换为 const char* 类型
     * @return 字符串的 const char* 表示
     */
    operator const char*() const {
        return data();
    }

    /**
     * @brief 隐式转换为 char* 类型
     * @return 字符串的 char* 表示
     */
    operator char*() {
        return data();
    }

    /**
     * @brief 隐式转换为 boolean 类型（非空为 true）
     * @return 字符串是否非空
     */
    operator bool() {
        return size() != 0;
    }

    /**
     * @brief 字符串索引访问操作符
     * @param index 索引位置
     * @return 索引位置的字符引用
     */
    char& operator[](c_size index) {
        return str_[index];
    }

    /**
     * @brief 字符串索引访问操作符（常量版本）
     * @param index 索引位置
     * @return 索引位置的字符常量引用
     */
    const char& operator[](c_size index) const {
        return str_[index];
    }

    /**
     * @brief 获取字符串的长度
     * @return 字符串的长度
     */
    c_size size() const {
        return len_;
    }

    /**
     * @brief 判断字符串是否为空
     * @return 是否为空
     */
    bool empty() const {
        return len_ == 0;
    }

    /**
     * @brief 获取字符串的起始地址
     * @return 字符串的起始地址
     */
    char* data() {
        return str_;
    }

    /**
     * @brief 获取字符串的起始地址（常量版本）
     * @return 字符串的起始地址
     */
    const char* data() const {
        return str_;
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的 CString 对象
     * @return 拼接后的新字符串
     */
    self operator+(const self& other) const {
        c_size mSize = this->size(), oSize = other.size();
        CString res{mSize + oSize};
        for (c_size i = 0; i < mSize; ++i) {
            res[i] = this->str_[i];
        }
        for (c_size i = 0; i < oSize; ++i) {
            res[mSize + i] = other.str_[i];
        }
        return res;
    }

    /**
     * @brief 转为字符串
     * @return 自身对象
     */
    CString __str__() const {
        return *this;
    }

    /**
     * @brief 计算字符串的哈希值
     * @return 字符串的哈希值
     */
    hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    /**
     * @brief 比较两个字符串
     * @param other 要比较的字符串
     * @return 比较结果
     */
    cmp_t __cmp__(const self& other) const {
        return std::strcmp(data(), other.data());
    }

    /**
     * @brief 判断两个字符串是否相等
     * @param other 要比较的字符串
     * @return 是否相等
     */
    bool __equals__(const self& other) const {
        return __cmp__(other) == 0;
    }

    /**
     * @brief 判断字符串与 C 风格字符串是否相等
     * @param other 要比较的 C 风格字符串
     * @return 是否相等
     */
    bool __equals__(const char* other) const {
        return std::strcmp(data(), other) == 0;
    }

    bool operator>(const self& other) const { return __cmp__(other) > 0; }

    bool operator<(const self& other) const { return __cmp__(other) < 0; }

    bool operator>=(const self& other) const { return __cmp__(other) >= 0; }

    bool operator<=(const self& other) const { return __cmp__(other) <= 0; }

    bool operator==(const self& other) const { return __cmp__(other) == 0; }

    bool operator!=(const self& other) const { return __cmp__(other) != 0; }

    bool operator==(const char* other) const { return __equals__(other); }

    bool operator!=(const char* other) const { return !__equals__(other); }

private:
    char* str_;  // 存储字符串的动态数组
    c_size len_; // 字符串的长度
};

/**
 * @brief 根据不同类型转换为 CString 对象（适用于自定义可打印类型）
 * @tparam T 自定义可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <MyPrintable T>
def cstr_impl(const T& value)->CString {
    return value.__str__();
}

/**
 * @brief 根据不同类型转换为 CString 对象（适用于标准可打印类型）
 * @tparam T 标准可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <StdPrintable T>
def cstr_impl(const T& value)->CString {
    std::stringstream stream;
    stream << value;
    return stream.str();
}

/**
 * @brief 根据不同类型自动选择合适的转换方法生成 CString 对象
 * @tparam T 可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <Printable T>
def cstr(const T& value)->CString {
    return cstr_impl(value);
}

/**
 * @brief 将 CString 对象转换为标准 C 风格字符串
 * @param value CString 对象
 * @return 标准 C 风格字符串
 */
inline const char* stdstr(const CString& value) {
    return value.data();
}

/**
 * @brief 将字符转换为整数（仅适用于 ASCII 数字字符）
 * @param ch 字符
 * @return 对应的整数值
 */
def c2i(char ch)->int {
    return ch - '0';
}

/**
 * @brief 将整数转换为对应字符（仅适用于 0-9 数字）
 * @param ch 整数
 * @return 对应的字符
 */
def i2c(int ch)->char {
    return ch + '0';
}

} // namespace my

/**
 * @brief 为 CString 类提供标准库格式化支持
 */
template <>
struct std::formatter<my::CString> : std::formatter<const char*> {
    auto format(const my::CString& value, auto& ctx) const {
        return std::formatter<const char*>::format(value.data(), ctx);
    }
};

/**
 * @brief 自定义字符串字面量，支持 `_cs` 后缀转换为 CString 对象
 * @param str C 风格字符串
 * @param len 字符串长度
 * @return 转换后的 CString 对象
 */
def operator""_cs(const char* str, size_t len)->my::CString {
    return my::CString{str, my::c_size(len)};
}

#endif // CSTRING_HPP