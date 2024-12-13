/**
 * @brief 静态数组
 * @author Ricky
 * @date 2024/12/2
 * @version 1.0
 */
#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "raise_error.hpp"
#include "ricky_memory.hpp"
#include "Sequence.hpp"

#include <initializer_list>

namespace my::util {

template <typename T>
class Array : public Sequence<Array<T>, T> {
    using self = Array<T>;
    using super = Sequence<Array<T>, T>;

public:
    using value_t = T;

    template <typename... Args>
    Array(c_size size, Args... args) :
            size_(size), arr_(my_alloc<T>(size_)) {
        for (c_size i = 0; i < size_; ++i) {
            my_construct(data() + i, args...);
        }
    }

    Array(std::initializer_list<T>&& initList) :
            size_(initList.size()), arr_(my_alloc<T>(size_)) {
        c_size idx = 0;
        for (const T& item : initList) {
            my_construct(data() + idx++, item);
        }
    }

    Array(const self& other) :
            size_(other.size_), arr_(my_alloc<T>(size_)) {
        for (c_size i = 0; i < size_; ++i) {
            my_construct(data() + i, other.data()[i]);
        }
    }

    Array(self&& other) noexcept :
            size_(other.size_), arr_(other.arr_) {
        other.size_ = 0;
        other.arr_ = nullptr;
    }

    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_destroy(arr_, size_);
        my_delloc(arr_);
        return *my_construct(this, other);
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        my_destroy(arr_, size_);
        my_delloc(arr_);

        this->size_ = other.size;
        this->arr_ = std::move(other.arr_);
        other.size_ = 0;
        other.arr_ = nullptr;
        return *this;
    }

    ~Array() {
        my_destroy(arr_, size_);
        my_delloc(arr_);
        size_ = 0;
    }

    value_t* data() {
        return arr_;
    }

    const value_t* data() const {
        return arr_;
    }

    value_t& at(c_size index) {
        return arr_[index];
    }

    const value_t& at(c_size index) const {
        return arr_[index];
    }

    c_size size() const {
        return size_;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (c_size i = 0; i < size_; ++i) {
            if (i != 0) stream << ", ";
            stream << at(i);
        }
        stream << ']';
        return stream.str();
    }

    /**
     * @brief 重新分配内存，不保留原有数据
     */
    template <typename... Args>
    void resize(c_size newSize, const Args&... args) {
        my_destroy(arr_, size_);
        my_construct(arr_, newSize, args...);
    }

    /**
     * @brief 分离数组，返回数组指针和大小，并将数组置空
     */
    std::pair<c_size, value_t*> separate() const {
        auto res = std::make_pair(size_, arr_);
        size_ = 0;
        arr_ = nullptr;
        return res;
    }

private:
    c_size size_;
    value_t* arr_;
};

} // namespace my::util

#endif // ARRAY_HPP