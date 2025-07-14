/**
 * @brief 内存块
 * @author Ricky
 * @date 2024/12/2
 * @version 1.0
 */
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "Array.hpp"

#include <cstring>

namespace my::util {

/**
 * @brief 内存块
 * @tparam T 元素类型
 * @tparam Alloc 内存分配器类型，默认为Allocator<T>
 */
template <typename T, typename Alloc = Allocator<T>>
class Buffer : public Sequence<Buffer<T>, T> {
public:
    using value_t = T;
    using Self = Buffer<value_t, Alloc>;
    using Super = Sequence<Buffer<value_t>, value_t>;

    using iterator = Super::iterator;
    using const_iterator = Super::const_iterator;

    /**
     * @brief 默认构造函数，创建一个空的Buffer
     */
    Buffer() :
            size_(0), capacity_(0), buf_(nullptr) {}

    /**
     * @brief 构造函数，创建一个指定容量的Buffer
     * @param capacity 初始容量
     */
    Buffer(usize capacity) :
            size_(0), capacity_(capacity), buf_(alloc_.allocate(capacity_)) {}

    /**
     * @brief 拷贝构造函数
     * @param other 另一个Buffer对象
     */
    Buffer(const Self& other) :
            alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_), buf_(alloc_.allocate(capacity_)) {
        for (usize i = 0; i < other.size(); ++i) {
            alloc_.construct(buf_ + i, other.buf_[i]);
        }
    }

    /**
     * @brief 移动构造函数
     * @param other 另一个Buffer对象
     */
    Buffer(Self&& other) noexcept :
            alloc_(other.alloc_), size_(other.size_), capacity_(other.capacity_), buf_(other.buf_) {
        other.size_ = other.capacity_ = 0;
        other.buf_ = nullptr;
    }

    /**
     * @brief 拷贝赋值
     * @param other 另一个Buffer对象
     * @return 返回当前对象的引用
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;
        alloc_.destroy(this);
        alloc_.construct(this, other);
        return *this;
    }

    /**
     * @brief 移动赋值
     * @param other 另一个Buffer对象
     * @return 返回当前对象的引用
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        alloc_.destroy(this);
        alloc_.construct(this, std::move(other));
        return *this;
    }

    /**
     * @brief 析构函数，释放内存
     */
    ~Buffer() {
        alloc_.destroy(buf_, size_);
        alloc_.deallocate(buf_, size_);
        size_ = capacity_ = 0;
    }

    /**
     * @brief 获取当前Buffer的大小
     */
    usize size() const {
        return size_;
    }

    /**
     * @brief 获取当前Buffer的容量
     */
    usize capacity() const {
        return capacity_;
    }

    /**
     * @brief 检查Buffer是否装满
     */
    bool full() const {
        return size_ == capacity_;
    }

    /**
     * @brief 获取内存块指针
     */
    value_t* data() {
        return buf_;
    }

    /**
     * @brief 获取内存块指针（常量版本）
     */
    const value_t* data() const {
        return buf_;
    }

    /**
     * @brief 获取首元
     */
    value_t& front() {
        return buf_[0];
    }

    /**
     * @brief 获取首元（常量版本）
     */
    const value_t& front() const {
        return buf_[0];
    }

    /**
     * @brief 获取尾元
     */
    value_t& back() {
        return buf_[size_ - 1];
    }

    /**
     * @brief 获取尾元（常量版本）
     */
    const value_t& back() const {
        return buf_[size_ - 1];
    }

    /**
     * @brief 设置大小，可能导致内存泄露，慎用
     */
    Self&& set_size(usize new_size) noexcept {
        this->size_ = new_size;
        return *this;
    }

    /**
     * @brief 设置容量，可能导致内存泄露，慎用
     */
    Self& set_capacity(usize new_cap) noexcept {
        this->capacity_ = new_cap;
        return *this;
    }

    /**
     * @brief 拷贝/移动追加，不会做容量检查
     * @param item 要追加的元素
     * @return 被追加元素的引用
     */
    template <typename U>
    value_t& append(U&& item) {
        alloc_.construct(buf_ + size_, std::forward<U>(item));
        return buf_[size_++];
    }

    /**
     * @brief 原位构造追加，不会做容量检查
     * @return 返回追加的元素
     */
    template <typename... Args>
    value_t& append(Args&&... args) {
        alloc_.construct(buf_ + size_, std::forward<Args>(args)...);
        return buf_[size_++];
    }

    /**
     * @brief 追加一块内存，不会做容量检查
     * @param ptr 指向要追加的内存块的指针
     * @param size 要追加的内存块大小
     */
    void append_bytes(const void* ptr, usize size) {
        std::memcpy(buf_ + size_, ptr, size);
        size_ += size;
    }

    /**
     * @brief 删除尾元
     */
    void pop_back() {
        alloc_.destroy(buf_ + size_);
        --size_;
    }

    /**
     * @brief 重新分配内存
     */
    void resize(usize new_capacity) {
        if (new_capacity == capacity_) return;
        alloc_.destroy(this);
        alloc_.construct(this, new_capacity);
    }

    /**
     * @brief 下标访问
     * @param index 索引
     * @return 返回指定索引的元素引用
     */
    value_t& at(usize index) {
        return buf_[index];
    }

    /**
     * @brief 下标访问（常量版本）
     * @param index 索引
     * @return 返回指定索引的元素引用
     */
    const value_t& at(usize index) const {
        return buf_[index];
    }

    /**
     * @brief 转换为Array，拷贝
     */
    Array<value_t> to_array() const {
        Array<value_t> arr(size_);
        for (usize i = 0; i < size_; ++i) {
            arr[i] = at(i);
        }
        return arr;
    }

    /**
     * @brief 转换为Array，移动
     */
    Array<value_t> to_array() {
        Array<value_t> arr(size_);
        for (usize i = 0; i < size_; ++i) {
            arr[i] = std::move(at(i));
        }
        size_ = 0;
        return arr;
    }

private:
    Alloc alloc_{};         // 内存分配器
    usize size_, capacity_; // 当前大小和容量
    value_t* buf_;          // 内存块指针
};

} // namespace my::util

#endif // BUFFER_HPP