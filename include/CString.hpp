/**
 * @brief 字符串，解决char*不能做返回值的问题
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef CSTRING_HPP
#define CSTRING_HPP

#include "hash.hpp"
#include "Allocator.hpp"
#include "Function.hpp"

#include <cctype>
#include <cstring>
#include <format>
#include <sstream>

namespace my {

/**
 * @brief 自定义字符串类，提供安全的字符串操作和内存管理
 */
template <typename Alloc = Allocator<char>>
class BaseCString {
public:
    using Self = BaseCString;

    /**
     * @brief 根据指定长度创建字符串
     * @param len 字符串的长度
     */
    BaseCString(usize len = 1) :
            str_(alloc_.allocate(len + 1)), len_(len) {
        std::memset(str_, 0, len + 1);
        str_[len_] = '\0';
    }

    /**
     * @brief 根据 C 风格字符串创建字符串
     * @param str C 风格字符串
     */
    BaseCString(const char* str) :
            Self(str, std::strlen(str)) {}

    /**
     * @brief 根据 C 风格字符串和指定长度创建字符串
     * @param str C 风格字符串
     * @param len 字符串的长度
     */
    BaseCString(const char* str, usize len) :
            Self(len) {
        std::memcpy(data(), str, len);
    }

    /**
     * @brief 根据 std::basic_string 创建字符串
     * @param str std::basic_string 对象
     */
    BaseCString(const std::basic_string<char>& str) :
            Self(str.c_str(), str.size()) {}

    /**
     * @brief 析构函数，释放动态分配的内存
     */
    ~BaseCString() {
        alloc_.deallocate(str_, len_ + 1);
    }

    /**
     * @brief 拷贝构造函数
     * @param other 要拷贝的 CString 对象
     */
    BaseCString(const Self& other) :
            Self(other.data(), other.size()) {}

    /**
     * @brief 移动构造函数
     * @param other 要移动的 CString 对象
     */
    BaseCString(Self&& other) noexcept :
            alloc_(std::move(other.alloc_)), str_(other.str_), len_(other.len_) {
        other.str_ = nullptr;
        other.len_ = 0;
    }

    /**
     * @brief 拷贝赋值操作符
     * @param other 要赋值的 CString 对象
     * @return 自身的引用
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        char* new_str = alloc_.allocate(other.len_ + 1);
        std::memcpy(new_str, other.str_, other.len_ + 1);
        alloc_.deallocate(str_, len_ + 1);

        str_ = new_str;
        len_ = other.len_;

        return *this;
    }

    /**
     * @brief 移动赋值操作符
     * @param other 要赋值的 CString 对象
     * @return 自身的引用
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        alloc_.deallocate(str_, len_ + 1);
        str_ = other.str_;
        len_ = other.len_;
        alloc_ = std::move(other.alloc_);

        other.str_ = nullptr;
        other.len_ = 0;

        return *this;
    }

    /**
     * @brief 创建单字符字符串
     */
    [[nodiscard]] static Self of(char ch) {
        Self str(1);
        str.str_[0] = ch;
        return str;
    }

    /**
     * @brief 创建长度为size的字符串，填充字符ch
     */
    [[nodiscard]] static Self of(usize size, char ch) {
        Self str(size);
        for (usize i = 0; i < size; ++i) {
            str[i] = ch;
        }
        return str;
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
    char& operator[](usize index) {
        return str_[index];
    }

    /**
     * @brief 字符串索引访问操作符（常量版本）
     * @param index 索引位置
     * @return 索引位置的字符常量引用
     */
    const char& operator[](usize index) const {
        return str_[index];
    }

    /**
     * @brief 获取字符串的长度
     * @return 字符串的长度
     */
    usize size() const {
        return len_;
    }

    usize length() const {
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
     * @brief 字符串切片，返回指定范围的子字符串 TODO CStringView类
     * @param start 起始索引
     * @param end 结束索引（不包含）
     * @return 子字符串
     */
    Self slice(usize start, isize end) const {
        auto m_size = size();
        start = neg_index(start, m_size);
        end = neg_index(end, static_cast<isize>(m_size));
        return Self{str_ + start, end - start};
    }

    /**
     * @brief 字符串切片，返回从指定索引开始到末尾的子字符串
     * @param start 起始索引
     * @return 子字符串
     */
    Self slice(usize start) const {
        return slice(start, size());
    }

    /**
     * @brief 查找指定字符，找不到返回npos
     */
    usize find(char ch) const {
        auto m_size = size();
        for (usize i = 0; i < m_size; ++i) {
            if (str_[i] == ch) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief 查找第一个不匹配的位置，若全部匹配，返回npos
     */
    usize find_first_not_of(char ch) const {
        auto m_size = size();
        for (usize i = 0; i < m_size; ++i) {
            if (str_[i] != ch) {
                return i;
            }
        }
        return npos;
    }

    /**
     * @brief 查找模式串的第一个匹配位置
     * @param pattern 模式串，长度为m
     * @param pos 起始查找位置（可选）
     * @return 模式串的第一个匹配位置，未找到返回 `npos`
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     */
    // usize find(const Self& pattern, usize pos = 0) const {
    //     return npos; // TODO
    // }

    // TODO find_all

    /**
     * @brief 检查字符串是否以指定子字符串开头
     * @param prefix 要检查的子字符串
     * @return 是否以指定子字符串开头
     */
    bool starts_with(const Self& prefix) const {
        if (size() < prefix.size()) {
            return false;
        }
        return slice(0, prefix.size()) == prefix;
    }

    /**
     * @brief 检查字符串是否以指定子字符串结尾
     * @param suffix 要检查的子字符串
     * @return 是否以指定子字符串结尾
     */
    bool ends_with(const Self& suffix) const {
        if (size() < suffix.size()) {
            return false;
        }
        return slice(size() - suffix.size()) == suffix;
    }

    /**
     * @brief 将字符串转换为全大写
     * @return 全大写的字符串
     */
    Self upper() const {
        Self res{*this};
        usize m_size = size();
        for (usize i = 0; i < m_size; ++i) {
            res[i] = std::toupper(res[i]);
        }
        return res;
    }

    /**
     * @brief 将字符串转换为全小写
     * @return 全小写的字符串
     */
    Self lower() const {
        Self res{*this};
        usize m_size = size();
        for (usize i = 0; i < m_size; ++i) {
            res[i] = std::tolower(res[i]);
        }
        return res;
    }

    /**
     * @brief 去除字符串首尾的空白字符
     * @return 去除空白后的字符串
     */
    Self trim() const {
        auto [l, r] = get_trim_index();
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的空白字符
     * @return 去除首部空白后的字符串
     */
    Self ltrim() const {
        return slice(get_ltrim_index());
    }

    /**
     * @brief 去除字符串尾部的空白字符
     * @return 去除尾部空白后的字符串
     */
    Self rtrim() const {
        return slice(get_rtrim_index());
    }

    /**
     * @brief 去除字符串首尾的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self trim(const Self& pattern) const {
        auto [l, r] = get_trim_index(pattern);
        return slice(l, r);
    }

    /**
     * @brief 去除字符串首部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self ltrim(const Self& pattern) const {
        return slice(get_ltrim_index(pattern));
    }

    /**
     * @brief 去除字符串尾部的指定模式
     * @param pattern 要去除的模式
     * @return 去除模式后的字符串
     */
    Self rtrim(const Self& pattern) const {
        return slice(get_rtrim_index(pattern));
    }

    Self remove_all(char ch) const {
        usize new_len = 0;
        for (usize i = 0; i < len_; ++i) {
            if (str_[i] != ch) {
                new_len++;
            }
        }

        Self result(new_len);
        usize pos = 0;
        for (usize i = 0; i < len_; ++i) {
            if (str_[i] != ch) {
                result[pos++] = str_[i];
            }
        }
        return result;
    }

    Self remove_all(Pred<char>&& pred) const {
        usize new_len = 0;
        for (usize i = 0; i < len_; ++i) {
            if (!pred(str_[i])) {
                new_len++;
            }
        }

        Self result(new_len);
        usize pos = 0;
        for (usize i = 0; i < len_; ++i) {
            if (!pred(str_[i])) {
                result[pos++] = str_[i];
            }
        }
        return result;
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的 CString 对象
     * @return 拼接后的新字符串
     */
    Self operator+(const Self& other) const {
        Self res(len_ + other.len_);
        std::memcpy(res.str_, str_, len_);
        std::memcpy(res.str_ + len_, other.str_, other.len_ + 1);
        return res;
    }

    Self operator+(const char* str) const {
        return *this + Self{str};
    }

    /**
     * @brief 转为字符串
     * @return 自身对象
     */
    [[nodiscard]] Self __str__() const {
        return *this;
    }

    /**
     * @brief 计算字符串的哈希值
     * @return 字符串的哈希值
     */
    [[nodiscard]] hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    /**
     * @brief 比较两个字符串
     * @param other 要比较的字符串
     * @return 比较结果
     */
    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return std::strcmp(data(), other.data());
    }

    /**
     * @brief 判断两个字符串是否相等
     * @param other 要比较的字符串
     * @return 是否相等
     */
    [[nodiscard]] bool __equals__(const Self& other) const {
        return __cmp__(other) == 0;
    }

    /**
     * @brief 判断字符串与 C 风格字符串是否相等
     * @param other 要比较的 C 风格字符串
     * @return 是否相等
     */
    [[nodiscard]] bool __equals__(const char* other) const {
        return std::strcmp(data(), other) == 0;
    }

    bool operator>(const Self& other) const { return __cmp__(other) > 0; }

    bool operator<(const Self& other) const { return __cmp__(other) < 0; }

    bool operator>=(const Self& other) const { return __cmp__(other) >= 0; }

    bool operator<=(const Self& other) const { return __cmp__(other) <= 0; }

    bool operator==(const Self& other) const { return __cmp__(other) == 0; }

    bool operator!=(const Self& other) const { return __cmp__(other) != 0; }

    bool operator==(const char* other) const { return __equals__(other); }

    bool operator!=(const char* other) const { return !__equals__(other); }

    /**
     * @brief 迭代器支持
     * @tparam IsConst 是否为常量迭代器
     */
    template <bool IsConst>
    class Iterator {
    public:
        using Self = Iterator<IsConst>;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::conditional_t<IsConst, const char, char>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        /**
         * @brief 构造一个迭代器
         * @param current 当前位置指针
         */
        explicit Iterator(pointer current) :
                current_(current) {}

        Iterator(const Self& other) = default;
        Self& operator=(const Self& other) = default;

        Iterator(Self&&) = default;
        Self& operator=(Self&&) = default;

        ~Iterator() = default;

        /**
         * @brief 解引用运算符
         * @return 返回当前元素的引用
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        reference operator*() {
            return *current_;
        }

        /**
         * @brief 解引用运算符（const 版本）
         * @return 返回当前元素的 const 引用
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        const_reference operator*() const {
            return *current_;
        }

        /**
         * @brief 获取指向当前元素的指针
         * @return 返回当前元素的指针
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        pointer operator->() {
            return current_;
        }

        /**
         * @brief 获取指向当前元素的指针（const 版本）
         * @return 返回当前元素的 const 指针
         * @note 如果迭代器无效（如超过范围），行为未定义
         */
        const_pointer operator->() const {
            return current_;
        }

        /**
         * @brief 前置自增运算符
         * 移动迭代器到下一个元素
         * @return 返回自增后的迭代器
         */
        Self& operator++() {
            ++current_;
            return *this;
        }

        /**
         * @brief 后置自增运算符
         * 移动迭代器到下一个元素
         * @return 返回自增前的迭代器
         */
        Self operator++(i32) {
            Self tmp(*this);
            ++tmp;
            return tmp;
        }

        /**
         * @brief 前置自减运算符
         * 移动迭代器到上一个元素
         * @return 返回自减后的迭代器
         */
        Self& operator--() {
            --current_;
            return *this;
        }

        /**
         * @brief 后置自减运算符
         * 移动迭代器到上一个元素
         * @return 返回自减前的迭代器
         */
        Self operator--(i32) {
            Self tmp(*this);
            --tmp;
            return tmp;
        }

        /**
         * @brief 比较两个迭代器是否相等
         * @param other 另一个迭代器
         * @return 如果相等返回 true，否则返回 false
         */
        bool operator==(const Self& other) const {
            return __equals__(other);
        }

        /**
         * @brief 比较两个迭代器是否不相等
         * @param other 另一个迭代器
         * @return 如果不相等返回 true，否则返回 false
         */
        bool operator!=(const Self& other) const {
            return !__equals__(other);
        }

        /**
         * @brief 比较两个迭代器的内部状态是否相等
         * @param other 另一个迭代器
         * @return 如果内部状态相等返回 true，否则返回 false
         */
        [[nodiscard]] bool __equals__(const Self& other) const {
            return this->current_ == other.current_;
        }

        /**
         * @brief 比较两个迭代器的顺序
         * @param other 另一个迭代器
         * @return 返回一个整数值，表示两个迭代器的顺序
         */
        [[nodiscard]] cmp_t __cmp__(const Self& other) const {
            return this->current_ - other.current_;
        }

    private:
        pointer current_;
    };

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    /**
     * @brief 获取动态数组的起始迭代器
     * @return 返回指向第一个元素的迭代器
     */
    iterator begin() {
        return iterator(str_);
    }

    /**
     * @brief 获取动态数组的起始迭代器（const 版本）
     * @return 返回指向第一个元素的 const 迭代器
     */
    const_iterator begin() const {
        return const_iterator(str_);
    }

    /**
     * @brief 获取动态数组的末尾迭代器
     * @return 返回指向最后一个元素之后的迭代器
     */
    iterator end() {
        return iterator(str_ + len_);
    }

    /**
     * @brief 获取动态数组的末尾迭代器（const 版本）
     * @return 返回指向最后一个元素之后的 const 迭代器
     */
    const_iterator end() const {
        return const_iterator(str_ + len_);
    }

private:
    /**
     * @brief 获取去除首尾空白后的索引范围
     * @return 首尾空白后的索引范围
     */
    std::pair<usize, usize> get_trim_index() const {
        usize l = 0, r = size();
        while (l < r && str_[l] == ' ') ++l;
        while (l < r && str_[r - 1] == ' ') --r;
        return std::make_pair(l, r);
    }

    /**
     * @brief 获取去除首尾模式后的索引范围
     * @param pattern 要去除的模式
     * @return 首尾模式后的索引范围
     */
    std::pair<usize, usize> get_trim_index(const Self& pattern) const {
        usize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return std::make_pair(l, r);
    }

    /**
     * @brief 获取去除首部空白后的索引
     * @return 去除首部空白后的索引
     */
    usize get_ltrim_index() const {
        usize l = 0, r = size();
        while (l < r && str_[l] == ' ') ++l;
        return l;
    }

    /**
     * @brief 获取去除首部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除首部模式后的索引
     */
    usize get_ltrim_index(const Self& pattern) const {
        usize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(l, l + p_size) == pattern) l += p_size;
        return l;
    }

    /**
     * @brief 获取去除尾部空白后的索引
     * @return 去除尾部空白后的索引
     */
    usize get_rtrim_index() const {
        usize l = 0, r = size();
        while (l < r && str_[r - 1] == ' ') --r;
        return r;
    }

    /**
     * @brief 获取去除尾部模式后的索引
     * @param pattern 要去除的模式
     * @return 去除尾部模式后的索引
     */
    usize get_rtrim_index(const Self& pattern) const {
        usize l = 0, r = size(), p_size = pattern.size();
        while (l + p_size <= r && slice(r - p_size, r) == pattern) r -= p_size;
        return r;
    }

private:
    Alloc alloc_{}; // 内存分配器
    char* str_;     // 存储字符串的动态数组
    usize len_;     // 字符串的长度
};

/**
 * @brief CString 类，使用默认分配器
 */
using CString = BaseCString<Allocator<char>>;

/**
 * @brief 根据不同类型转换为 CString 对象（适用于自定义可打印类型）
 * @tparam T 自定义可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <MyPrintable T>
fn cstr_impl(const T& value)->CString {
    return value.__str__();
}

/**
 * @brief 根据不同类型转换为 CString 对象（适用于标准可打印类型）
 * @tparam T 标准可打印类型
 * @param value 要转换的值
 * @return 转换后的 CString 对象
 */
template <StdPrintable T>
fn cstr_impl(const T& value)->CString {
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
fn cstr(const T& value)->CString {
    return cstr_impl(value);
}

/**
 * @brief 将 CString 对象转换为标准 C 风格字符串
 * @param value CString 对象
 * @return 标准 C 风格字符串
 */
fn stdstr(const CString& value)->const char* {
    return value.data();
}

/**
 * @brief 将字符转换为整数（仅适用于 ASCII 数字字符）
 * @param ch 字符
 * @return 对应的整数值
 */
fn c2i(char ch)->i32 {
    return ch - '0';
}

/**
 * @brief 将整数转换为对应字符（仅适用于 0-9 数字）
 * @param ch 整数
 * @return 对应的字符
 */
fn i2c(i32 ch)->char {
    return ch + '0';
}

} // namespace my

/**
 * @brief 为 CString 类提供标准库格式化支持
 */
template <>
struct std::formatter<my::CString> : std::formatter<const char*> {
    fn format(const my::CString& value, auto& ctx) const {
        return std::formatter<const char*>::format(value.data(), ctx);
    }
};

namespace my {

/**
 * @brief 自定义字符串字面量，支持 `_cs` 后缀转换为 CString 对象
 * @param str C 风格字符串
 * @param len 字符串长度
 * @return 转换后的 CString 对象
 */
fn operator""_cs(const char* str, size_t len)->CString {
    return CString{str, len};
}

} // namespace my

#endif // CSTRING_HPP