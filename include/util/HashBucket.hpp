/**
 * @brief 哈希桶，使用Robin哈希实现
 * @author Ricky
 * @date 2024/12/18
 * @version 1.0
 */
#ifndef HASH_BUCKET_HPP
#define HASH_BUCKET_HPP

#include "hash.hpp"
#include "Array.hpp"
#include "RelationIterator.hpp"

namespace my::util {

/**
 * @brief 哈希桶接口
 */
template <typename T>
class HashBucket : public Object<HashBucket<T>> {
    using self = HashBucket<T>;

public:
    using value_t = T;

    HashBucket() = default;

    virtual ~HashBucket() = default;

    /**
     * @brief 容量
     */
    virtual isize capacity() const = 0;

    /**
     * @brief 克隆自身
     */
    virtual self* clone() const = 0;

    /**
     * @brief 尝试根据hash值获取元素
     * @return 返回指针，如果没有则返回nullptr
     */
    virtual value_t* tryGet(hash_t hashVal) = 0;

    virtual const value_t* tryGet(hash_t hashVal) const = 0;

    /**
     * @brief 根据hash值删除元素
     */
    virtual void pop(hash_t hashVal) = 0;

    /**
     * @brief 扩容
     */
    virtual void expand(isize newCapacity) noexcept = 0;

    /**
     * @brief 清空hash桶
     */
    virtual void clear() = 0;

    /**
     * @brief 将hash值转换为索引
     */
    virtual isize hash2index(hash_t hashVal) const {
        return hashVal % capacity();
    }

    /**
     * @brief 判断是否存在
     * @return true=存在 false=不存在
     */
    virtual bool contains(hash_t hashVal) const {
        return tryGet(hashVal) != nullptr;
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
    using self = RobinManager<T>;

public:
    using value_t = T; // 值的类型

    /**
     * @brief 默认构造函数。
     * 初始化一个未被管理的管理器。
     */
    RobinManager() :
            moveDist_(MOVED_NOTHING), hashVal_(0), value_() {}

    /**
     * @brief 构造函数。
     * 初始化一个管理器，指定值、哈希值和移动距离。
     * @param value 要存储的值。
     * @param hashVal 哈希值。
     * @param moveDist 移动距离。
     */
    RobinManager(const value_t& value, hash_t hashVal, u32 moveDist) :
            moveDist_(moveDist), hashVal_(hashVal), value_(value) {}

    /**
     * @brief 拷贝构造函数。
     * @param other 需要拷贝的管理器。
     */
    RobinManager(const self& other) :
            moveDist_(other.moveDist_), hashVal_(other.hashVal_), value_(other.value_) {}

    /**
     * @brief 移动构造函数。
     * @param other 需要移动的管理器。
     */
    RobinManager(self&& other) noexcept :
            moveDist_(other.moveDist_), hashVal_(other.hashVal_), value_(std::move(other.value_)) {
        other.unmanage();
    }

    /**
     * @brief 拷贝赋值操作符。
     * @param other 需要拷贝的管理器。
     * @return 本管理器对象的引用。
     */
    self& operator=(const self& other) {
        if (this == &other) return *this;

        this->moveDist_ = other.moveDist_;
        this->hashVal_ = other.hashVal_;
        this->value_ = other.value_;
        return *this;
    }

    /**
     * @brief 移动赋值操作符。
     * @param other 需要移动的管理器。
     * @return 本管理器对象的引用。
     */
    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->moveDist_ = other.moveDist_;
        this->hashVal_ = other.hashVal_;
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
    hash_t hashVal() const {
        return hashVal_;
    }

    /**
     * @brief 判断当前管理器是否被管理。
     * @return 如果管理器被管理返回 true，否则返回 false。
     */
    bool isManaged() const {
        return moveDist_ != MOVED_NOTHING;
    }

    /**
     * @brief 取消管理当前管理器。
     */
    void unmanage() {
        moveDist_ = MOVED_NOTHING;
    }

    /**
     * @brief 判断哈希值是否相等。
     * @param hashVal 需要比较的哈希值。
     * @return 如果哈希值相等返回 true，否则返回 false。
     */
    bool hashEqual(hash_t hashVal) const {
        return this->hashVal_ == hashVal;
    }

    /**
     * @brief 判断当前管理器的移动距离是否大于另一个管理器。
     * @param other 需要比较的管理器。
     * @return 如果当前管理器的移动距离更大返回 true，否则返回 false。
     */
    bool moveGt(const self& other) const {
        return this->moveDist_ > other.moveDist_;
    }

    /**
     * @brief 判断当前管理器的移动距离是否大于指定值。
     * @param moveDist 需要比较的移动距离。
     * @return 如果当前管理器的移动距离更大返回 true，否则返回 false。
     */
    bool moveGt(int moveDist) const {
        return this->moveDist_ > moveDist;
    }

    /**
     * @brief 判断当前管理器的移动距离是否小于或等于另一个管理器。
     * @param other 需要比较的管理器。
     * @return 如果当前管理器的移动距离小于或等于另一个管理器返回 true，否则返回 false。
     */
    bool moveLe(const self& other) const {
        return this->moveDist_ <= other.moveDist_;
    }

    /**
     * @brief 判断当前管理器的移动距离是否小于或等于指定值。
     * @param moveDist 需要比较的移动距离。
     * @return 如果当前管理器的移动距离小于或等于指定值返回 true，否则返回 false。
     */
    bool moveLe(int moveDist) const {
        return this->moveDist_ <= moveDist;
    }

    /**
     * @brief 增加当前管理器的移动距离。
     * @param d 增加的距离，默认为 1。
     */
    void addMoveDist(int d = 1) {
        moveDist_ += d;
    }

    /**
     * @brief 与另一个管理器交换数据。
     * @param other 需要交换的管理器。
     */
    void swap(self& other) {
        std::swap(this->moveDist_, other.moveDist_);
        std::swap(this->hashVal_, other.hashVal_);
        std::swap(this->value_, other.value_);
    }

private:
    int moveDist_;   // 目标桶与实际存放桶之间的距离
    hash_t hashVal_; // 哈希值
    value_t value_;  // 维护的值

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
    using self = RobinHashBucket<T>;
    using super = HashBucket<T>;

public:
    using value_t = T;                 // 值的类型
    using manager_t = RobinManager<T>; // 管理器类型

    /**
     * @brief 默认构造函数。
     * 初始化一个空哈希桶。
     */
    RobinHashBucket(isize size = 0) :
            robinManagers_(size) {}

    /**
     * @brief 拷贝构造函数。
     * @param other 需要拷贝的哈希桶。
     */
    RobinHashBucket(const self& other) :
            robinManagers_(other.robinManagers_) {}

    /**
     * @brief 移动构造函数。
     * @param other 需要移动的哈希桶。
     */
    RobinHashBucket(self&& other) noexcept :
            robinManagers_(std::move(other.robinManagers_)) {}

    /**
     * @brief 拷贝赋值操作符。
     * @param other 需要拷贝的哈希桶。
     * @return 本哈希桶对象的引用。
     */
    self& operator=(const self& other) {
        if (this == &other) return *this;

        this->robinManagers_ = other.robinManagers_;
        return *this;
    }

    /**
     * @brief 移动赋值操作符。
     * @param other 需要移动的哈希桶。
     * @return 本哈希桶对象的引用。
     */
    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->robinManagers_ = std::move(other.robinManagers_);
        return *this;
    }

    /**
     * @brief 获取哈希桶的容量。
     * @return 返回哈希桶的容量。
     */
    isize capacity() const override {
        return robinManagers_.size();
    }

    /**
     * @brief 克隆当前哈希桶。
     * @return 返回克隆后的哈希桶指针。
     */
    virtual self* clone() const override {
        return new self(*this);
    }

    /**
     * @brief 根据哈希值获取对应的管理器地址。
     * 1. 找到相同的hash值, 返回该管理器地址
     * 2. 找到距离最近的空闲管理器, 返回该管理器地址
     * 3. 没有空闲且相同hash值的管理器，返回nullptr
     * @param hashVal 哈希值。
     * @return 返回管理器地址，如果没有找到返回 nullptr。
     */
    manager_t* tryGetManager(hash_t hashVal) {
        isize m_capacity = capacity();
        isize idx = super::hash2index(hashVal);
        for (isize i = 0; i < m_capacity; ++i) {
            auto& manager = robinManagers_.at((idx + i) % m_capacity);
            if (!manager.isManaged() || manager.hashEqual(hashVal)) {
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
     * @param hashVal 哈希值。
     * @return 返回常量管理器地址，如果没有找到返回 nullptr。
     */
    const manager_t* tryGetManager(hash_t hashVal) const {
        isize m_capacity = capacity();
        isize idx = super::hash2index(hashVal);
        for (isize i = 0; i < m_capacity; ++i) {
            const auto& manager = robinManagers_.at((idx + i) % m_capacity);
            if (!manager.isManaged() || manager.hashEqual(hashVal)) {
                return &manager;
            }
        }
        return nullptr;
    }

    /**
     * @brief 根据哈希值获取对应的值。
     * @param hashVal 哈希值。
     * @return 返回值的指针，如果没有找到返回 nullptr。
     */
    value_t* tryGet(hash_t hashVal) override {
        auto* manager = tryGetManager(hashVal);
        if (manager == nullptr || !manager->isManaged()) {
            return nullptr;
        }
        return &manager->value();
    }

    /**
     * @brief 根据哈希值获取对应的值（常量版本）。
     * @param hashVal 哈希值。
     * @return 返回值的常量指针，如果没有找到返回 nullptr。
     */
    const value_t* tryGet(hash_t hashVal) const override {
        auto* manager = tryGetManager(hashVal);
        if (manager == nullptr || !manager->isManaged()) {
            return nullptr;
        }
        return &manager->value();
    }

    /**
     * @brief 根据哈希值删除对应的键值对。
     * @param hashVal 哈希值。
     */
    void pop(hash_t hashVal) override {
        isize m_capacity = capacity();
        auto* manager = tryGetManager(hashVal);
        if (manager == nullptr || !manager->isManaged()) {
            return;
        }

        isize curIdx = std::distance(&robinManagers_.at(0), manager);
        while (true) {
            isize nextIdx = (curIdx + 1) % m_capacity;
            auto& curManager = robinManagers_.at(curIdx);
            auto& nextManager = robinManagers_.at(nextIdx);
            if (!nextManager.isManaged() || nextManager.moveLe(0)) {
                curManager.unmanage();
                break;
            }
            curManager = std::move(nextManager);
            ++curIdx;
        }
    }

    /**
     * @brief 在给定哈希值位置设置值，新设置的值会优先放在目标桶上。
     * @tparam V 值的类型。
     * @param value 值。
     * @param hashVal 哈希值。
     * @return 返回设置值的指针。
     */
    template <typename V>
    value_t* setValue(V&& value, hash_t hashVal) {
        manager_t valueManager{std::forward<V>(value), hashVal, 0};
        isize m_capacity = capacity();
        isize idx = super::hash2index(hashVal);
        for (isize i = 0; i < m_capacity; ++i) {
            auto& manager = robinManagers_.at((idx + i) % m_capacity);
            if (!manager.isManaged()) {
                manager = std::move(valueManager);
                return &manager.value();
            }

            if (valueManager.moveGt(manager)) {
                valueManager.swap(manager);
            }
            valueManager.addMoveDist();
        }
        RuntimeError("RobinHashBucket set_value failed, bucket is full");
        return nullptr;
    }

    /**
     * @brief 扩展哈希桶的大小。
     * @param newCapacity 新的容量。
     */
    void expand(isize newCapacity) noexcept override {
        Array<manager_t> tmpManager{std::move(robinManagers_)};
        robinManagers_.resize(newCapacity);
        for (auto&& manager : tmpManager) {
            if (manager.isManaged()) {
                setValue(manager.value(), manager.hashVal());
            }
        }
    }

    /**
     * @brief 清空哈希桶。
     */
    void clear() override {
        robinManagers_.resize(0);
    }

    template <bool IsConst>
    class RobinHashBucketIterator : public Object<RobinHashBucketIterator<IsConst>> {
        using self = RobinHashBucketIterator<IsConst>;

    public:
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
         * @param bucketPtr 指向哈希桶的指针。
         * @param index 初始索引。
         */
        RobinHashBucketIterator(container_t* bucketPtr = nullptr, isize index = 0) :
                bucketPtr_(bucketPtr), index_(index) {}

        /**
         * @brief 拷贝构造函数。
         * @param other 需要拷贝的迭代器。
         */
        RobinHashBucketIterator(const self& other) :
                bucketPtr_(other.bucketPtr_), index_(other.index_) {}

        /**
         * @brief 拷贝赋值操作符。
         * @param other 需要拷贝的迭代器。
         * @return 返回本迭代器对象的引用。
         */
        self& operator=(const self& other) {
            if (this == &other) return *this;

            this->bucketPtr_ = other.bucketPtr_;
            this->index_ = other.index_;
            return *this;
        }

        /**
         * @brief 解引用运算符。
         * @return 返回当前值的引用。
         */
        reference operator*() {
            return bucketPtr_->at(index_).value();
        }

        /**
         * @brief 解引用运算符（常量版本）。
         * @return 返回当前值的常量引用。
         */
        const_reference operator*() const {
            return bucketPtr_->at(index_).value();
        }

        /**
         * @brief 获取指针。
         * @return 返回当前值的指针。
         */
        pointer operator->() {
            return &bucketPtr_->at(index_).value();
        }

        /**
         * @brief 获取指针（常量版本）。
         * @return 返回当前值的常量指针。
         */
        const_pointer operator->() const {
            return &bucketPtr_->at(index_).value();
        }

        /**
         * @brief 前置自增运算符。
         * 移动迭代器到下一个有效的键值对。
         * @return 返回自增后的迭代器。
         */
        self& operator++() {
            ++index_;
            isize m_size = bucketPtr_->size();
            while (index_ < m_size && !bucketPtr_->at(index_).isManaged()) {
                ++index_;
            }
            return *this;
        }

        /**
         * @brief 后置自增运算符。
         * 移动迭代器到下一个有效的键值对。
         * @return 返回自增前的迭代器。
         */
        self operator++(int) {
            self tmp{*this};
            ++tmp;
            return tmp;
        }

        /**
         * @brief 比较两个迭代器是否相等。
         * @param other 另一个迭代器。
         * @return 如果相等返回 true，否则返回 false。
         */
        bool __equals__(const self& other) const {
            return this->bucketPtr_ == other.bucketPtr_ && this->index_ == other.index_;
        }

    private:
        container_t* bucketPtr_;
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
            if (robinManagers_.at(i).isManaged()) {
                return iterator{&robinManagers_, i};
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
            if (robinManagers_.at(i).isManaged()) {
                return const_iterator{&robinManagers_, i};
            }
        }
        return end();
    }

    /**
     * @brief 获取哈希桶的末尾迭代器。
     * @return 返回末尾迭代器。
     */
    iterator end() {
        return iterator{&robinManagers_, capacity()};
    }

    /**
     * @brief 获取哈希桶的末尾迭代器（常量版本）。
     * @return 返回末尾迭代器。
     */
    const_iterator end() const {
        return const_iterator{&robinManagers_, capacity()};
    }

private:
    Array<manager_t> robinManagers_;
};

} // namespace my::util

#endif // HASH_BUCKET_HPP