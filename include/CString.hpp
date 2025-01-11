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

class CString {
    using self = CString;

public:
    CString() :
            str_(my_alloc<char>(1)) {
        std::memset(str_, 0, 1);
    }

    CString(c_size len) :
            str_(my_alloc<char>(len + 1)) {
        std::memset(str_, 0, len + 1);
    }

    CString(const char* str) :
            CString(str, std::strlen(str)) {}

    CString(const char* str, c_size len) :
            CString(len) {
        std::memcpy(data(), str, len);
    }

    CString(const std::basic_string<char>& str) :
            CString(str.c_str(), str.size()) {}

    ~CString() {
        my_delloc(str_);
    }

    CString(const self& other) :
            CString(other.data(), other.size()) {}

    CString(self&& other) noexcept :
            str_(other.str_) {
        other.str_ = nullptr;
    }

    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_delloc(str_);
        return *my_construct(this, other);
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        my_delloc(str_);
        return *my_construct(this, std::forward<self>(other));
    }

    operator const char*() const {
        return data();
    }

    operator char*() {
        return data();
    }

    operator bool() {
        return size() != 0;
    }

    char& operator[](c_size index) {
        return str_[index];
    }

    const char& operator[](c_size index) const {
        return str_[index];
    }

    c_size size() const {
        return static_cast<c_size>(std::strlen(data()));
    }

    bool empty() const {
        return size() == 0;
    }

    char* data() {
        return str_;
    }

    const char* data() const {
        return str_;
    }

    CString __str__() const {
        return *this;
    }

    hash_t __hash__() const {
        return bytes_hash(data(), size());
    }

    cmp_t __cmp__(const self& other) const {
        return std::strcmp(data(), other.data());
    }

    bool __equals__(const self& other) const {
        return __cmp__(other) == 0;
    }

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
    char* str_;
};

template <MyPrintable T>
inline CString cstr_impl(const T& value) {
    return value.__str__();
}

template <StdPrintable T>
inline CString cstr_impl(const T& value) {
    std::stringstream stream;
    stream << value;
    return stream.str();
}

template <Printable T>
inline CString cstr(const T& value) {
    return cstr_impl(value);
}

inline const char* stdstr(const CString& value) {
    return value.data();
}

} // namespace my

template <>
struct std::formatter<my::CString> : std::formatter<const char*> {
    auto format(const my::CString& value, auto& ctx) const {
        return std::formatter<const char*>::format(value.data(), ctx);
    }
};

def operator""_cs(const char* str, size_t len)->my::CString {
    return my::CString{str, my::c_size(len)};
}

#endif // CSTRING_HPP