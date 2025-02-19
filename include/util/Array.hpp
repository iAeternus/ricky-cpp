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
#include "Pair.hpp"

#include <initializer_list>

namespace my::util {

/**
 * @class Array
 * @brief 静态数组类，提供固定大小的数组功能。
 *
 * 静态数组是指数组的大小在创建时确定，并且不能动态改变，除非调用 `resize` 方法重新分配内存。
 * 该类提供了数组的基本操作，如初始化、拷贝、移动、析构、元素访问、范围生成等。
 *
 * @tparam T 数组元素的类型。
 */
template <typename T>
class Array : public Sequence<Array<T>, T> {
    using self = Array<T>;
    using super = Sequence<Array<T>, T>;

public:
    using value_t = T;

    /**
     * @brief 构造一个指定大小的静态数组，并初始化所有元素。
     * @tparam Args 元素的类型。
     * @param size 数组的大小。
     * @param args 用于初始化数组元素的参数。
     */
    template <typename... Args>
    Array(c_size size, const Args&... args) :
            size_(size), arr_(my_alloc<T>(size_)) {
        for (c_size i = 0; i < size_; ++i) {
            my_construct(data() + i, args...);
        }
    }

    /**
     * @brief 使用初始化列表构造静态数组。
     * @param initList 初始化列表，包含数组的初始元素。
     */
    Array(std::initializer_list<T>&& initList) :
            size_(initList.size()), arr_(my_alloc<T>(size_)) {
        c_size idx = 0;
        for (const T& item : initList) {
            my_construct(data() + idx++, item);
        }
    }

    /**
     * @brief 拷贝构造函数。
     * @param other 需要拷贝的静态数组。
     */
    Array(const self& other) :
            size_(other.size_), arr_(my_alloc<T>(size_)) {
        for (c_size i = 0; i < size_; ++i) {
            my_construct(data() + i, other.data()[i]);
        }
    }

    /**
     * @brief 移动构造函数。
     * @param other 需要移动的静态数组。
     */
    Array(self&& other) noexcept :
            size_(other.size_), arr_(other.arr_) {
        other.size_ = 0;
        other.arr_ = nullptr;
    }

    /**
     * @brief 拷贝赋值操作符。
     * @param other 需要拷贝的静态数组。
     * @return 本静态数组对象的引用。
     */
    self& operator=(const self& other) {
        if (this == &other) return *this;

        my_destroy(arr_, size_);
        my_delloc(arr_);
        return *my_construct(this, other);
    }

    /**
     * @brief 移动赋值操作符。
     * @param other 需要移动的静态数组。
     * @return 本静态数组对象的引用。
     */
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

    /**
     * @brief 析构函数。
     * 销毁数组中的所有元素并释放内存。
     */
    ~Array() {
        my_destroy(arr_, size_);
        my_delloc(arr_);
        size_ = 0;
    }

    /**
     * @brief 获取数组的指针。
     * @return 返回数组的指针。
     * @note 返回的指针可以直接访问数组的元素，但使用时需要确保索引在有效范围内。
     */
    value_t* data() {
        return arr_;
    }

    /**
     * @brief 获取数组的指针（常量版本）。
     * @return 返回数组的常量指针。
     * @note 返回的指针可以直接访问数组的元素，但使用时需要确保索引在有效范围内。
     */
    const value_t* data() const {
        return arr_;
    }

    /**
     * @brief 通过索引访问数组元素。
     * @param index 索引。
     * @return 返回对应元素的引用。
     * @note 不会对索引进行边界检查，如果索引超出范围，可能会导致未定义行为。
     */
    value_t& at(c_size index) {
        return arr_[index];
    }

    /**
     * @brief 通过索引访问数组元素（常量版本）。
     * @param index 索引。
     * @return 返回对应元素的常量引用。
     * @note 不会对索引进行边界检查，如果索引超出范围，可能会导致未定义行为。
     */
    const value_t& at(c_size index) const {
        return arr_[index];
    }

    /**
     * @brief 获取数组的大小。
     * @return 返回数组的大小。
     */
    c_size size() const {
        return size_;
    }

    /**
     * @brief 检查数组是否为空。
     * @return 如果数组为空，返回 true；否则返回 false。
     */
    bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief 获取数组的字符串表示。
     * @return 返回数组的 CSV 格式的字符串。
     */
    CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (c_size i = 0; i < size_; ++i) {
            if (i != 0) stream << ',';
            stream << at(i);
        }
        stream << ']';
        return CString{stream.str()};
    }

    /**
     * @brief 重新分配内存，不保留原有数据。
     * @tparam Args 用于初始化数组元素的参数。
     * @param newSize 新的数组大小。
     * @param args 用于初始化新数组的参数。
     * @note 调用此方法会销毁原数组中的所有元素并释放内存。
     */
    template <typename... Args>
    void resize(c_size newSize, const Args&... args) {
        my_destroy(this);
        my_construct(this, newSize, args...);
    }

    /**
     * @brief 分离数组，返回数组指针和大小，并将数组置空。
     * @return 返回包含数组大小和指针的 Pair。
     * @note 分离后，原数组将不再管理数组的内存，用户需要手动管理返回的指针。
     */
    Pair<c_size, value_t*> separate() {
        auto res = Pair{size_, arr_};
        size_ = 0;
        arr_ = nullptr;
        return res;
    }

private:
    c_size size_;  // 静态数组长度
    value_t* arr_; // 指向静态数组首地址的指针
};

/**
 * @class Range
 * @brief 生成一个范围迭代器。
 *
 * Range 类用于生成一个范围迭代器，可以用于遍历从 `start` 到 `end` 的整数序列。
 * 迭代器支持增、减操作，并且可以生成可迭代范围。
 */
class Range : public Object<Range> {
public:
    /**
     * @brief 构造一个范围迭代器。
     * @param start 起始值。
     * @param end 结束值（不包含）。
     * @param step 步长。
     */
    Range(c_size start, c_size end, c_size step = 1) :
            start_(start), end_(end), step_(step) {}

    /**
     * @brief 构造一个范围迭代器。
     * 默认起始值为 0，步长为 1。
     * @param end 结束值（不包含）。
     */
    Range(c_size end) :
            Range(0, end, 1) {}

    /**
     * @brief 获取范围的大小。
     * @return 返回范围的大小。
     */
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

        /**
         * @brief 构造一个范围迭代器。
         * @param current 当前值。
         * @param step 步长。
         */
        RangeIterator(c_size current, c_size step) :
                current_(current), step_(step) {}

        /**
         * @brief 拷贝构造函数。
         * @param other 需要拷贝的范围迭代器。
         */
        RangeIterator(const self& other) :
                current_(other.current_), step_(other.step_) {}

        /**
         * @brief 解引用运算符。
         * @return 返回当前值的引用。
         */
        reference operator*() {
            return current_;
        }

        /**
         * @brief 获取指针。
         * @return 返回当前值的指针。
         */
        pointer operator->() {
            return &current_;
        }

        /**
         * @brief 解引用运算符（常量版本）。
         * @return 返回当前值的常量引用。
         */
        const_reference operator*() const {
            return current_;
        }

        /**
         * @brief 获取常量指针。
         * @return 返回当前值的常量指针。
         */
        const_pointer operator->() const {
            return &current_;
        }

        /**
         * @brief 前置自增运算符。
         * 将当前值增加步长。
         * @return 返回自增后的迭代器。
         */
        self& operator++() {
            current_ += step_;
            return *this;
        }

        /**
         * @brief 后置自增运算符。
         * 将当前值增加步长，并返回自增前的迭代器。
         * @return 返回自增前的迭代器。
         */
        self operator++(int) {
            self tmp(*this);
            ++tmp;
            return tmp;
        }

        /**
         * @brief 前置自减运算符。
         * 将当前值减少步长。
         * @return 返回自减后的迭代器。
         */
        self& operator--() {
            current_ -= step_;
            return *this;
        }

        /**
         * @brief 后置自减运算符。
         * 将当前值减少步长，并返回自减前的迭代器。
         * @return 返回自减前的迭代器。
         */
        self operator--(int) {
            self tmp(*this);
            --tmp;
            return tmp;
        }

        /**
         * @brief 比较两个迭代器是否相等。
         * @param other 另一个迭代器。
         * @return 如果相等返回 true，否则返回 false。
         */
        bool __equals__(const self& other) const {
            return current_ == other.current_;
        }

    private:
        c_size current_, step_;
    };

private:
    c_size start_, end_, step_; // 起点、终点、步长
};

/**
 * @brief 计算可迭代对象的总和。
 * @tparam I 可迭代对象的类型。
 * @tparam Init 初始值的类型。
 * @param obj 可迭代对象。
 * @param init 初始值。
 * @return 返回总和。
 */
template <Iterable I, typename Init>
def sum(I&& obj, Init init = Init{})->Init {
    for (auto&& elem : obj) {
        init += elem;
    }
    return init;
}

} // namespace my::util

#endif // ARRAY_HPP