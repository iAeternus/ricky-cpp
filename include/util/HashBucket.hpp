/**
 * @brief 哈希桶，使用Robin哈希实现
 * @author Ricky
 * @date 2024/12/18
 * @version 1.0
 */
#ifndef HASH_BUCKET_HPP
#define HASH_BUCKET_HPP

#include "Array.hpp"

namespace my::util {

/**
 * @brief 哈希桶接口
 */
template <typename T>
class HashBucket : public Object<HashBucket<T>> {
public:
    using value_t = T;
    using Self = HashBucket<value_t>;

    HashBucket() = default;

    virtual ~HashBucket() = default;

    /**
     * @brief 容量
     */
    virtual isize capacity() const = 0;

    /**
     * @brief 克隆自身
     */
    virtual Self* clone() const = 0;

    /**
     * @brief 尝试根据hash值获取元素
     * @return 返回指针，如果没有则返回nullptr
     */
    virtual value_t* try_get(hash_t hash_val) = 0;

    virtual const value_t* try_get(hash_t hash_val) const = 0;

    /**
     * @brief 根据hash值删除元素
     */
    virtual void pop(hash_t hash_val) = 0;

    /**
     * @brief 扩容
     */
    virtual void expand(isize new_capacity) noexcept = 0;

    /**
     * @brief 清空hash桶
     */
    virtual void clear() = 0;

    /**
     * @brief 将hash值转换为索引
     */
    virtual isize hash2index(hash_t hash_val) const {
        return hash_val % capacity();
    }

    /**
     * @brief 判断是否存在
     * @return true=存在 false=不存在
     */
    virtual bool contains(hash_t hash_val) const {
        return try_get(hash_val) != nullptr;
    }
};

/**
 * @class RobinManager
 * @brief Robin哈希桶的管理器，负责存储和管理键值对及相关信息。
 * @see https://sf-zhou.github.io/programming/robin_hood_hashing.html
 *
 * RobinManager 是一个模板类，用于维护哈希桶中的每个元素的移动距离、哈希值和存储的值。
 * 它通过移动距离来处理哈希冲突，确保哈希表的性能和效率。
 *
 * @tparam T 存储的值类型。
 */
template <typename T>
class RobinManager : public Object<RobinManager<T>> {
public:
    using value_t = T;
    using Self = RobinManager<value_t>;

    /**
     * @brief 默认构造函数。
     * 初始化一个未被管理的管理器。
     */
    RobinManager() :
            move_dist_(MOVED_NOTHING), hash_val_(0), value_() {}

    /**
     * @brief 构造函数。
     * 初始化一个管理器，指定值、哈希值和移动距离。
     * @param value 要存储的值。
     * @param hash_val 哈希值。
     * @param move_dist 移动距离。
     */
    RobinManager(const value_t& value, hash_t hash_val, u32 move_dist) :
            move_dist_(move_dist), hash_val_(hash_val), value_(value) {}

    /**
     * @brief 拷贝构造函数。
     * @param other 需要拷贝的管理器。
     */
    RobinManager(const Self& other) :
            move_dist_(other.move_dist_), hash_val_(other.hash_val_), value_(other.value_) {}

    /**
     * @brief 移动构造函数。
     * @param other 需要移动的管理器。
     */
    RobinManager(Self&& other) noexcept :
            move_dist_(other.move_dist_), hash_val_(other.hash_val_), value_(std::move(other.value_)) {
        other.unmanage();
    }

    /**
     * @brief 拷贝赋值操作符。
     * @param other 需要拷贝的管理器。
     * @return 本管理器对象的引用。
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        this->move_dist_ = other.move_dist_;
        this->hash_val_ = other.hash_val_;
        this->value_ = other.value_;
        return *this;
    }

    /**
     * @brief 移动赋值操作符。
     * @param other 需要移动的管理器。
     * @return 本管理器对象的引用。
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->move_dist_ = other.move_dist_;
        this->hash_val_ = other.hash_val_;
        this->value_ = std::move(other.value_);
        other.unmanage();
        return *this;
    }

    /**
     * @brief 获取存储的值。
     * @return 返回值的引用。
     */
    value_t& value() {
        return value_;
    }

    /**
     * @brief 获取存储的值（常量版本）。
     * @return 返回值的常量引用。
     */
    const value_t& value() const {
        return value_;
    }

    /**
     * @brief 获取哈希值。
     * @return 返回哈希值。
     */
    hash_t hash_val() const {
        return hash_val_;
    }

    /**
     * @brief 判断当前管理器是否被管理。
     * @return 如果管理器被管理返回 true，否则返回 false。
     */
    bool is_managed() const {
        return move_dist_ != MOVED_NOTHING;
    }

    /**
     * @brief 取消管理当前管理器。
     */
    void unmanage() {
        move_dist_ = MOVED_NOTHING;
    }

    /**
     * @brief 判断哈希值是否相等。
     * @param hash_val 需要比较的哈希值。
     * @return 如果哈希值相等返回 true，否则返回 false。
     */
    bool hash_eq(hash_t hash_val) const {
        return this->hash_val_ == hash_val;
    }

    /**
     * @brief 判断当前管理器的移动距离是否大于另一个管理器。
     * @param other 需要比较的管理器。
     * @return 如果当前管理器的移动距离更大返回 true，否则返回 false。
     */
    bool move_gt(const Self& other) const {
        return this->move_dist_ > other.move_dist_;
    }

    /**
     * @brief 判断当前管理器的移动距离是否大于指定值。
     * @param move_dist 需要比较的移动距离。
     * @return 如果当前管理器的移动距离更大返回 true，否则返回 false。
     */
    bool move_gt(int move_dist) const {
        return this->move_dist_ > move_dist;
    }

    /**
     * @brief 判断当前管理器的移动距离是否小于或等于另一个管理器。
     * @param other 需要比较的管理器。
     * @return 如果当前管理器的移动距离小于或等于另一个管理器返回 true，否则返回 false。
     */
    bool move_le(const Self& other) const {
        return this->move_dist_ <= other.move_dist_;
    }

    /**
     * @brief 判断当前管理器的移动距离是否小于或等于指定值。
     * @param move_dist 需要比较的移动距离。
     * @return 如果当前管理器的移动距离小于或等于指定值返回 true，否则返回 false。
     */
    bool move_le(int move_dist) const {
        return this->move_dist_ <= move_dist;
    }

    /**
     * @brief 增加当前管理器的移动距离。
     * @param d 增加的距离，默认为 1。
     */
    void add_move_dist(int d = 1) {
        move_dist_ += d;
    }

    /**
     * @brief 与另一个管理器交换数据。
     * @param other 需要交换的管理器。
     */
    void swap(Self& other) {
        std::swap(this->move_dist_, other.move_dist_);
        std::swap(this->hash_val_, other.hash_val_);
        std::swap(this->value_, other.value_);
    }

private:
    int move_dist_;   // 目标桶与实际存放桶之间的距离
    hash_t hash_val_; // 哈希值
    value_t value_;   // 维护的值

    constexpr static int MOVED_NOTHING = INT_MAX; // 表示未被管理的特殊值
};

/**
 * @class RobinHashBucket
 * @brief 使用 Robin Hood 算法实现的哈希桶。
 *
 * RobinHashBucket 是一个基于 Robin Hood 哈希算法实现的哈希桶，提供了高效的键值对存储和检索功能。
 * 它通过维护每个桶的管理距离来处理哈希冲突，确保良好的性能和较低的延迟。
 *
 * @tparam T 存储的值类型。
 */
template <typename T>
class RobinHashBucket : public HashBucket<T> {
public:
    using value_t = T;
    using Self = RobinHashBucket<value_t>;
    using Super = HashBucket<value_t>;
    using manager_t = RobinManager<value_t>;

    /**
     * @brief 默认构造函数。
     * 初始化一个空哈希桶。
     */
    RobinHashBucket(isize size = 0) :
            robin_managers_(size) {}

    /**
     * @brief 拷贝构造函数。
     * @param other 需要拷贝的哈希桶。
     */
    RobinHashBucket(const Self& other) :
            robin_managers_(other.robin_managers_) {}

    /**
     * @brief 移动构造函数。
     * @param other 需要移动的哈希桶。
     */
    RobinHashBucket(Self&& other) noexcept :
            robin_managers_(std::move(other.robin_managers_)) {}

    /**
     * @brief 拷贝赋值操作符。
     * @param other 需要拷贝的哈希桶。
     * @return 本哈希桶对象的引用。
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        this->robin_managers_ = other.robin_managers_;
        return *this;
    }

    /**
     * @brief 移动赋值操作符。
     * @param other 需要移动的哈希桶。
     * @return 本哈希桶对象的引用。
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->robin_managers_ = std::move(other.robin_managers_);
        return *this;
    }

    /**
     * @brief 获取哈希桶的容量。
     * @return 返回哈希桶的容量。
     */
    isize capacity() const override {
        return robin_managers_.size();
    }

    /**
     * @brief 克隆当前哈希桶。
     * @return 返回克隆后的哈希桶指针。
     */
    virtual Self* clone() const override {
        return new Self(*this);
    }

    /**
     * @brief 根据哈希值获取对应的管理器地址。
     * 1. 找到相同的hash值, 返回该管理器地址
     * 2. 找到距离最近的空闲管理器, 返回该管理器地址
     * 3. 没有空闲且相同hash值的管理器，返回nullptr
     * @param hash_val 哈希值。
     * @return 返回管理器地址，如果没有找到返回 nullptr。
     */
    manager_t* try_get_manager(hash_t hash_val) {
        isize m_capacity = capacity();
        isize idx = Super::hash2index(hash_val);
        for (isize i = 0; i < m_capacity; ++i) {
            auto& manager = robin_managers_.at((idx + i) % m_capacity);
            if (!manager.is_managed() || manager.hash_eq(hash_val)) {
                return &manager;
            }
        }
        return nullptr;
    }

    /**
     * @brief 根据哈希值获取对应的管理器地址（常量版本）。
     * 1. 找到相同的hash值, 返回该管理器地址
     * 2. 找到距离最近的空闲管理器, 返回该管理器地址
     * 3. 没有空闲且相同hash值的管理器，返回nullptr
     * @param hash_val 哈希值。
     * @return 返回常量管理器地址，如果没有找到返回 nullptr。
     */
    const manager_t* try_get_manager(hash_t hash_val) const {
        isize m_capacity = capacity();
        isize idx = Super::hash2index(hash_val);
        for (isize i = 0; i < m_capacity; ++i) {
            const auto& manager = robin_managers_.at((idx + i) % m_capacity);
            if (!manager.is_managed() || manager.hash_eq(hash_val)) {
                return &manager;
            }
        }
        return nullptr;
    }

    /**
     * @brief 根据哈希值获取对应的值。
     * @param hash_val 哈希值。
     * @return 返回值的指针，如果没有找到返回 nullptr。
     */
    value_t* try_get(hash_t hash_val) override {
        auto* manager = try_get_manager(hash_val);
        if (manager == nullptr || !manager->is_managed()) {
            return nullptr;
        }
        return &manager->value();
    }

    /**
     * @brief 根据哈希值获取对应的值（常量版本）。
     * @param hash_val 哈希值。
     * @return 返回值的常量指针，如果没有找到返回 nullptr。
     */
    const value_t* try_get(hash_t hash_val) const override {
        auto* manager = try_get_manager(hash_val);
        if (manager == nullptr || !manager->is_managed()) {
            return nullptr;
        }
        return &manager->value();
    }

    /**
     * @brief 根据哈希值删除对应的键值对。
     * @param hash_val 哈希值。
     */
    void pop(hash_t hash_val) override {
        isize m_capacity = capacity();
        auto* manager = try_get_manager(hash_val);
        if (manager == nullptr || !manager->is_managed()) {
            return;
        }

        isize cur_idx = std::distance(&robin_managers_.at(0), manager);
        while (true) {
            isize next_idx = (cur_idx + 1) % m_capacity;
            auto& cur_manager = robin_managers_.at(cur_idx);
            auto& next_manager = robin_managers_.at(next_idx);
            if (!next_manager.is_managed() || next_manager.move_le(0)) {
                cur_manager.unmanage();
                break;
            }
            cur_manager = std::move(next_manager);
            ++cur_idx;
        }
    }

    /**
     * @brief 在给定哈希值位置设置值，新设置的值会优先放在目标桶上。
     * @tparam V 值的类型。
     * @param value 值。
     * @param hash_val 哈希值。
     * @return 返回设置值的指针。
     */
    template <typename V>
    value_t* set_value(V&& value, hash_t hash_val) {
        manager_t value_manager{std::forward<V>(value), hash_val, 0};
        isize m_capacity = capacity();
        isize idx = Super::hash2index(hash_val);
        for (isize i = 0; i < m_capacity; ++i) {
            auto& manager = robin_managers_.at((idx + i) % m_capacity);
            if (!manager.is_managed()) {
                manager = std::move(value_manager);
                return &manager.value();
            }

            if (value_manager.move_gt(manager)) {
                value_manager.swap(manager);
            }
            value_manager.add_move_dist();
        }
        RuntimeError("RobinHashBucket set_value failed, bucket is full");
        return nullptr;
    }

    /**
     * @brief 扩展哈希桶的大小。
     * @param new_capacity 新的容量。
     */
    void expand(isize new_capacity) noexcept override {
        Array<manager_t> tmp_manager{std::move(robin_managers_)};
        robin_managers_.resize(new_capacity);
        for (auto&& manager : tmp_manager) {
            if (manager.is_managed()) {
                set_value(manager.value(), manager.hash_val());
            }
        }
    }

    /**
     * @brief 清空哈希桶。
     */
    void clear() override {
        robin_managers_.resize(0);
    }

    template <bool IsConst>
    class RobinHashBucketIterator : public Object<RobinHashBucketIterator<IsConst>> {
    public:
        using Self = RobinHashBucketIterator<IsConst>;

        using container_t = std::conditional_t<IsConst, const Array<manager_t>, Array<manager_t>>;
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::conditional_t<IsConst, const value_t, value_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        /**
         * @brief 构造一个迭代器。
         * @param bucket_ptr 指向哈希桶的指针。
         * @param index 初始索引。
         */
        RobinHashBucketIterator(container_t* bucket_ptr = nullptr, isize index = 0) :
                bucket_ptr_(bucket_ptr), index_(index) {}

        /**
         * @brief 拷贝构造函数。
         * @param other 需要拷贝的迭代器。
         */
        RobinHashBucketIterator(const Self& other) :
                bucket_ptr_(other.bucket_ptr_), index_(other.index_) {}

        /**
         * @brief 拷贝赋值操作符。
         * @param other 需要拷贝的迭代器。
         * @return 返回本迭代器对象的引用。
         */
        Self& operator=(const Self& other) {
            if (this == &other) return *this;

            this->bucket_ptr_ = other.bucket_ptr_;
            this->index_ = other.index_;
            return *this;
        }

        /**
         * @brief 解引用运算符。
         * @return 返回当前值的引用。
         */
        reference operator*() {
            return bucket_ptr_->at(index_).value();
        }

        /**
         * @brief 解引用运算符（常量版本）。
         * @return 返回当前值的常量引用。
         */
        const_reference operator*() const {
            return bucket_ptr_->at(index_).value();
        }

        /**
         * @brief 获取指针。
         * @return 返回当前值的指针。
         */
        pointer operator->() {
            return &bucket_ptr_->at(index_).value();
        }

        /**
         * @brief 获取指针（常量版本）。
         * @return 返回当前值的常量指针。
         */
        const_pointer operator->() const {
            return &bucket_ptr_->at(index_).value();
        }

        /**
         * @brief 前置自增运算符。
         * 移动迭代器到下一个有效的键值对。
         * @return 返回自增后的迭代器。
         */
        Self& operator++() {
            ++index_;
            isize m_size = bucket_ptr_->size();
            while (index_ < m_size && !bucket_ptr_->at(index_).is_managed()) {
                ++index_;
            }
            return *this;
        }

        /**
         * @brief 后置自增运算符。
         * 移动迭代器到下一个有效的键值对。
         * @return 返回自增前的迭代器。
         */
        Self operator++(int) {
            Self tmp{*this};
            ++tmp;
            return tmp;
        }

        /**
         * @brief 比较两个迭代器是否相等。
         * @param other 另一个迭代器。
         * @return 如果相等返回 true，否则返回 false。
         */
        bool __equals__(const Self& other) const {
            return this->bucket_ptr_ == other.bucket_ptr_ && this->index_ == other.index_;
        }

    private:
        container_t* bucket_ptr_;
        isize index_;
    };

    using iterator = RobinHashBucketIterator<false>;
    using const_iterator = RobinHashBucketIterator<true>;

    /**
     * @brief 获取哈希桶的起始迭代器。
     * @return 返回起始迭代器。
     */
    iterator begin() {
        isize m_capacity = capacity();
        for (isize i = 0; i < m_capacity; ++i) {
            if (robin_managers_.at(i).is_managed()) {
                return iterator{&robin_managers_, i};
            }
        }
        return end();
    }

    /**
     * @brief 获取哈希桶的起始迭代器（常量版本）。
     * @return 返回起始迭代器。
     */
    const_iterator begin() const {
        isize m_capacity = capacity();
        for (isize i = 0; i < m_capacity; ++i) {
            if (robin_managers_.at(i).is_managed()) {
                return const_iterator{&robin_managers_, i};
            }
        }
        return end();
    }

    /**
     * @brief 获取哈希桶的末尾迭代器。
     * @return 返回末尾迭代器。
     */
    iterator end() {
        return iterator{&robin_managers_, capacity()};
    }

    /**
     * @brief 获取哈希桶的末尾迭代器（常量版本）。
     * @return 返回末尾迭代器。
     */
    const_iterator end() const {
        return const_iterator{&robin_managers_, capacity()};
    }

private:
    Array<manager_t> robin_managers_;
};

} // namespace my::util

#endif // HASH_BUCKET_HPP