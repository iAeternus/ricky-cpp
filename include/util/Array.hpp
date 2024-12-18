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

/**
 * @brief 静态数组
 */
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

        this->size_ = other.size_;
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

    bool empty() const {
        return size_ == 0;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (c_size i = 0; i < size_; ++i) {
            if (i != 0) stream << ",";
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

/**
 * @brief 区间
 */
class Range : public Object<Range> {
public:
    Range(c_size start, c_size end, c_size step = 1) :
            start_(start), end_(end), step_(step) {}

    Range(c_size end) :
            Range(0, end, 1) {}

    c_size size() const {
        return (end_ - start_ + step_ - 1) / step_;
    }

    class RangeIterator : public Object<RangeIterator> {
        using self = RangeIterator;
        using super = Object<self>;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = c_size;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        RangeIterator(c_size current, c_size step) :
                current_(current), step_(step) {}

        RangeIterator(const self& other) :
                current_(other.current_), step_(other.step_) {}

        reference operator*() {
            return current_;
        }

        pointer operator->() {
            return &current_;
        }

        const_reference operator*() const {
            return current_;
        }

        const_pointer operator->() const {
            return &current_;
        }

        self& operator++() {
            current_ += step_;
            return *this;
        }

        self operator++(int) {
            self tmp(*this);
            ++tmp;
            return tmp;
        }

        self& operator--() {
            current_ -= step_;
            return *this;
        }

        self operator--(int) {
            self tmp(*this);
            --tmp;
            return tmp;
        }

        bool __equals__(const self& other) const {
            return current_ == other.current_;
        }

    private:
        c_size current_, step_;
    };

private:
    c_size start_, end_, step_;
};

template <Iterable I, typename Init>
def sum(I&& obj, Init init = Init{})->Init {
    for (auto&& elem : obj) {
        init += elem;
    }
    return init;
}

} // namespace my::util

#endif // ARRAY_HPP