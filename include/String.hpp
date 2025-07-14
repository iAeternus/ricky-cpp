/**
 * @brief 字符串，支持SSO/多种编码
 * @author Ricky
 * @date 2025/7/14
 * @version 2.0
 */
#ifndef STRING_HPP
#define STRING_HPP

#include "Allocator.hpp"
#include "Object.hpp"
#include "Encoding.hpp"
#include "util/Sequence.hpp"
#include <cstring>
#include <utility>

namespace tmp {

using namespace my; // TODO delete

template <typename T>
struct MemBlock {
    T* ptr;
    usize cap;
};

template <typename T = char, typename Alloc = Allocator<T>>
class Store : public Object<Store<T, Alloc>> {
public:
    using value_t = T;
    using Self = Store<value_t, Alloc>;

    /**
     * @brief 默认构造函数，创建空字符串
     */
    Store() :
            length_(0), is_sso_(true) {
        sso_[0] = '\0';
    }

    /**
     * @brief 通过字符串指针和长度构造
     * @param str 字符串
     * @param len 字符串长度
     */
    Store(const char* str, usize len) {
        alloc(len);
        append(str, len);
    }

    /**
     * @brief 拷贝构造
     */
    Store(const Store& other) {
        alloc(other.length_);
        append(other.data(), other.length_);
    }

    /**
     * @brief 移动构造
     */
    Store(Store&& other) noexcept :
            length_(other.length_), is_sso_(other.is_sso_) {
        if (is_sso_) {
            std::memcpy(sso_, other.sso_, SSO_MAX + 1);
        } else {
            heap_ = other.heap_;
            other.is_sso_ = true;
            other.sso_[0] = '\0';
            other.length_ = 0;
        }
    }

    /**
     * @brief 析构函数
     */
    ~Store() {
        free();
    }

    /**
     * @brief 拷贝赋值
     */
    Store& operator=(const Store& other) {
        if (this != &other) {
            free();
            alloc(other.length_);
            append(other.data(), other.length_);
        }
        return *this;
    }

    /**
     * @brief 移动赋值
     */
    Store& operator=(Store&& other) noexcept {
        if (this != &other) {
            free();
            length_ = other.length_;
            is_sso_ = other.is_sso_;

            if (is_sso_) {
                std::memcpy(sso_, other.sso_, SSO_MAX + 1);
            } else {
                heap_ = other.heap_;
            }

            other.is_sso_ = true;
            other.sso_[0] = '\0';
            other.length_ = 0;
        }
        return *this;
    }

    /**
     * @brief 获取字符串指针
     */
    const value_t* data() const noexcept {
        return is_sso_ ? sso_ : heap_.ptr;
    }

    /**
     * @brief 获取字符串长度
     */
    usize length() const noexcept {
        return length_;
    }

    /**
     * @brief 在字符串末尾追加字符串
     */
    void append(const value_t* str, usize len) {
        if (len == 0) return;

        auto new_len = length_ + len;
        if (is_sso_) {
            if (new_len > SSO_MAX) {
                sso2heap(new_len); // 从SSO切换到堆
            }
        } else if (new_len > heap_.cap) {
            // 扩展堆空间
            auto new_cap = std::max(new_len << 1, heap_.cap << 1);
            value_t* new_ptr = alloc_.allocate(new_cap + 1);
            std::memcpy(new_ptr, heap_.ptr, length_);

            alloc_.deallocate(heap_.ptr, heap_.cap);
            heap_.ptr = new_ptr;
            heap_.cap = new_cap;
        }

        auto* dest = data();
        std::memcpy(dest, str, len);
        length_ = new_len;
        dest[length_] = '\0';
    }

    /**
     * @brief 清空存储
     */
    void clear() noexcept {
        free();
        length_ = 0;
        is_sso_ = true;
        sso_[0] = '\0';
    }

private:
    /**
     * @brief 分配内存，生成空字符串
     * @param size 申请的内存大小
     */
    void alloc(usize size) {
        if (size <= SSO_MAX) {
            is_sso_ = true;
        } else {
            is_sso_ = false;
            heap_.ptr = alloc_.allocate(size + 1);
        }
        length_ = 0;
        data()[0] = '\0';
    }

    /**
     * @brief 释放内存
     */
    void free() noexcept {
        if (!is_sso_) {
            alloc_.deallocate(heap_.ptr, heap_.cap);
        }
    }

    /**
     * @brief 从SSO切换到堆
     */
    void sso2heap(usize new_len) {
        value_t tmp[SSO_MAX + 1];
        std::memcpy(tmp, sso_, length_ + 1);

        heap_.ptr = alloc_.allocate((new_len << 1) + 1);
        heap_.cap = new_len << 1;
        std::memcpy(heap_.ptr, tmp, length_);

        is_sso_ = false;
    }

private:
    static constexpr usize SSO_MAX = 15; // 小字符串优化阈值

    Alloc alloc_{}; // 内存分配器
    usize length_;  // 字符串长度
    bool is_sso_;   // 是否为SSO
    union {
        MemBlock<value_t> heap_;   // 堆内存块
        value_t sso_[SSO_MAX + 1]; // 栈内存快
    };
};

/**
 * @class BaseString
 * @brief 字符串，支持SSO/多种编码
 * @tparam Alloc 内存分配器
 */
template <typename T = char, typename Alloc = Allocator<T>>
class BaseString : public util::Sequence<BaseString<Alloc>, T> {
public:
    using value_t = T;
    using Self = BaseString<value_t, Alloc>;
    using Super = util::Sequence<Self, T>;

    /**
     * @brief 默认构造函数，创建一个空字符串，并指定编码
     * @param encoding 字符串的编码（可选）
     */
    BaseString(Encoding enc = Encoding::UTF8) :
            encoding_(enc) {}

    /**
     * @brief 使用字符串和编码构造
     * @param str 字符串
     * @param enc 编码
     */
    BaseString(const char* str, Encoding enc = Encoding::UTF8) :
            encoding_(enc), store_(str, std::strlen(str)) {}

    /**
     * @brief 构造函数，根据 C 风格字符串创建字符串
     * @param str C 风格字符串
     * @param length 字符串的长度
     * @param enc 字符串的编码（可选）
     */
    BaseString(const char* str, usize length, Encoding enc = Encoding::UTF8) :
            encoding_(enc), store_(str, length) {}

    /**
     * @brief 构造函数，根据自定义字符串创建字符串
     * @param cstr 自定义字符串
     * @param encoding 字符串的编码（可选）
     */
    BaseString(const CString& cstr, const CString& encoding = UTF8) :
            BaseString(cstr.data(), cstr.size(), encoding) {}

    /**
     * @brief 使用标准字符串和编码构造
     * @param str 标准字符串
     * @param enc 编码
     */
    BaseString(const std::string& str, Encoding enc = Encoding::UTF8) :
            encoding_(enc), store_(str.data(), str.size()) {}

    /**
     * @brief 默认析构函数
     */
    ~BaseString() = default;

    /**
     * @brief 获取编码
     * @return 编码
     */
    Encoding encoding() const noexcept {
        return encoding_;
    }

    /**
     * @brief 设置编码
     * @param new_encoding 新编码
     */
    void set_encoding(Encoding new_encoding) {
        convert_to(new_encoding);
    }

    /**
     * @brief 获取字符串长度
     * @return 字符串长度
     */
    usize size() const noexcept {
        return store_.size();
    }

    /**
     * @brief 获取字符串长度，等效于size()
     * @return 字符串长度
     */
    usize length() const noexcept {
        return store_.length();
    }

    /**
     * @brief 判断字符串是否为空
     * @return true=是 false=否
     */
    bool empty() const noexcept {
        return store_.length() == 0;
    }

    /**
     * @brief 获取字符串指针
     */
    const char* data() const noexcept {
        return store_.data();
    }

    /**
     * @brief 清空字符串
     */
    void clear() {
        store_.clear();
    }

    /**
     * @brief 追加字符串到末尾
     * @param other 另一个字符串
     */
    template <typename S>
    Self& append(const S& other) {
        if (this->encoding_ != other.encoding_) {
            Self tmp(other);
            tmp.set_encoding(encoding_);
            store_.append(tmp.data(), tmp.length());
        } else {
            store_.append(other.data(), other.length());
        }
        return *this;
    }

    /**
     * @brief 字符串拼接操作符
     * @param other 要拼接的字符串对象
     * @return 拼接后的新字符串
     */
    Self operator+(const Self& other) const {
        Self result = *this;
        result.append(other);
        return result;
    }

    /**
     * @brief 累加字符串拼接操作符
     * @param other 要拼接的字符串对象
     * @return 自身的引用
     */
    Self& operator+=(const Self& other) {
        return append(other);
    }

private:
    Encoding encoding_;           // 字符串编码
    Store<value_t, Alloc> store_; // 字符串内部存储
};

/**
 * @brief 字符串类型别名
 */
using String = BaseString<char, Allocator<char>>;

} // namespace tmp

#endif // STRING_HPP