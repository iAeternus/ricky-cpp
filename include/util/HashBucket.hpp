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
    virtual c_size capacity() const = 0;

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
    virtual void expand(c_size newCapacity) noexcept = 0;

    /**
     * @brief 清空hash桶
     */
    virtual void clear() = 0;

    /**
     * @brief 将hash值转换为索引
     */
    virtual c_size hash2index(hash_t hashVal) const {
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
 * @brief Robin hash
 * @see https://sf-zhou.github.io/programming/robin_hood_hashing.html
 */
template <typename T>
class RobinManager : public Object<RobinManager<T>> {
    using self = RobinManager<T>;

public:
    using value_t = T;

    RobinManager() :
            moveDist_(MOVED_NOTHING), hashVal_(0), value_() {}

    RobinManager(const value_t& value, hash_t hashVal, u32 moveDist) :
            moveDist_(moveDist), hashVal_(hashVal), value_(value) {}

    RobinManager(const self& other) :
            moveDist_(other.moveDist_), hashVal_(other.hashVal_), value_(other.value_) {}

    RobinManager(self&& other) noexcept :
            moveDist_(other.moveDist_), hashVal_(other.hashVal_), value_(std::move(other.value_)) {
        other.unmanage();
    }

    self& operator=(const self& other) {
        if (this == &other) return *this;

        this->moveDist_ = other.moveDist_;
        this->hashVal_ = other.hashVal_;
        this->value_ = other.value_;
        return *this;
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->moveDist_ = other.moveDist_;
        this->hashVal_ = other.hashVal_;
        this->value_ = std::move(other.value_);
        other.unmanage();
        return *this;
    }

    value_t& value() {
        return value_;
    }

    const value_t& value() const {
        return value_;
    }

    hash_t hashVal() const {
        return hashVal_;
    }

    /**
     * @brief 判断是否被管理
     * @return true=是 false=否
     */
    bool isManaged() const {
        return moveDist_ != MOVED_NOTHING;
    }

    /**
     * @brief 取消管理
     */
    void unmanage() {
        moveDist_ = MOVED_NOTHING;
    }

    /**
     * @brief 判断哈希值相等
     * @return true=相等 false=不相等
     */
    bool hashEqual(hash_t hashVal) const {
        return this->hashVal_ == hashVal;
    }

    /**
     * @brief 移动距离是否大于other
     * @return true=是 false=否
     */
    bool moveGt(const self& other) const {
        return this->moveDist_ > other.moveDist_;
    }

    bool moveGt(int moveDist) const {
        return this->moveDist_ > moveDist;
    }

    bool moveLe(const self& other) const {
        return this->moveDist_ <= other.moveDist_;
    }

    bool moveLe(int moveDist) const {
        return this->moveDist_ <= moveDist;
    }

    void addMoveDist(int d = 1) {
        moveDist_ += d;
    }

    void swap(self& other) {
        std::swap(this->moveDist_, other.moveDist_);
        std::swap(this->hashVal_, other.hashVal_);
        std::swap(this->value_, other.value_);
    }

private:
    int moveDist_;   // 目标桶与实际存放桶之间的距离
    hash_t hashVal_; // hash值
    value_t value_;  // 维护的值

    constexpr static int MOVED_NOTHING = INT_MAX;
};

/**
 * @brief Robin哈希桶
 */
template <typename T>
class RobinHashBucket : public HashBucket<T> {
    using self = RobinHashBucket<T>;
    using super = HashBucket<T>;

public:
    using value_t = T;
    using manager_t = RobinManager<T>;

    RobinHashBucket(c_size size = 0) :
            robinManagers_(size) {}

    RobinHashBucket(const self& other) :
            robinManagers_(other.robinManagers_) {}

    RobinHashBucket(self&& other) noexcept :
            robinManagers_(std::move(other.robinManagers_)) {}

    self& operator=(const self& other) {
        if (this == &other) return *this;

        this->robinManagers_ = other.robinManagers_;
        return *this;
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->robinManagers_ = std::move(other.robinManagers_);
        return *this;
    }

    c_size capacity() const override {
        return robinManagers_.size();
    }

    virtual self* clone() const override {
        return new self(*this);
    }

    /**
     * @brief 获取合适的管理器地址
     * 1. 找到相同的hash值, 返回该管理器地址
     * 2. 找到距离最近的空闲管理器, 返回该管理器地址
     * 3. 没有空闲且相同hash值的管理器，返回nullptr
     */
    manager_t* tryGetManager(hash_t hashVal) {
        c_size m_capacity = capacity();
        c_size idx = super::hash2index(hashVal);
        for (c_size i = 0; i < m_capacity; ++i) {
            auto& manager = robinManagers_.at((idx + i) % m_capacity);
            if (!manager.isManaged() || manager.hashEqual(hashVal)) {
                return &manager;
            }
        }
        return nullptr;
    }

    const manager_t* tryGetManager(hash_t hashVal) const {
        c_size m_capacity = capacity();
        c_size idx = super::hash2index(hashVal);
        for (c_size i = 0; i < m_capacity; ++i) {
            const auto& manager = robinManagers_.at((idx + i) % m_capacity);
            if (!manager.isManaged() || manager.hashEqual(hashVal)) {
                return &manager;
            }
        }
        return nullptr;
    }

    value_t* tryGet(hash_t hashVal) override {
        auto* manager = tryGetManager(hashVal);
        if (manager == nullptr || !manager->isManaged()) {
            return nullptr;
        }
        return &manager->value();
    }

    const value_t* tryGet(hash_t hashVal) const override {
        auto* manager = tryGetManager(hashVal);
        if (manager == nullptr || !manager->isManaged()) {
            return nullptr;
        }
        return &manager->value();
    }

    void pop(hash_t hashVal) override {
        c_size m_capacity = capacity();
        auto* manager = tryGetManager(hashVal);
        if (manager == nullptr || !manager->isManaged()) {
            return;
        }

        c_size curIdx = std::distance(&robinManagers_.at(0), manager);
        while (true) {
            c_size nextIdx = (curIdx + 1) % m_capacity;
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
     * @brief 在给定hash值位置设置值，新设置的值会优先放在目标桶上
     */
    template <typename V>
    value_t* setValue(V&& value, hash_t hashVal) {
        manager_t valueManager{std::forward<V>(value), hashVal, 0};
        c_size m_capacity = capacity();
        c_size idx = super::hash2index(hashVal);
        for (c_size i = 0; i < m_capacity; ++i) {
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

    void expand(c_size newCapacity) noexcept override {
        Array<manager_t> tmpManager{std::move(robinManagers_)};
        robinManagers_.resize(newCapacity);
        for (auto&& manager : tmpManager) {
            if (manager.isManaged()) {
                setValue(manager.value(), manager.hashVal());
            }
        }
    }

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

        RobinHashBucketIterator(container_t* bucketPtr = nullptr, c_size index = 0) :
                bucketPtr_(bucketPtr), index_(index) {}

        RobinHashBucketIterator(const self& other) :
                bucketPtr_(other.bucketPtr_), index_(other.index_) {}

        self& operator=(const self& other) {
            if (this == &other) return *this;

            this->bucketPtr_ = other.bucketPtr_;
            this->index_ = other.index_;
            return *this;
        }

        reference operator*() {
            return bucketPtr_->at(index_).value();
        }

        const_reference operator*() const {
            return bucketPtr_->at(index_).value();
        }

        pointer operator->() {
            return &bucketPtr_->at(index_).value();
        }

        const_pointer operator->() const {
            return &bucketPtr_->at(index_).value();
        }

        self& operator++() {
            ++index_;
            c_size m_size = bucketPtr_->size();
            while (index_ < m_size && !bucketPtr_->at(index_).isManaged()) {
                ++index_;
            }
            return *this;
        }

        self operator++(int) {
            self tmp{*this};
            ++tmp;
            return tmp;
        }

        bool __equals__(const self& other) const {
            return this->bucketPtr_ == other.bucketPtr_ && this->index_ == other.index_;
        }

    private:
        container_t* bucketPtr_;
        c_size index_;
    };

    using iterator = RobinHashBucketIterator<false>;
    using const_iterator = RobinHashBucketIterator<true>;

    iterator begin() {
        c_size m_capacity = capacity();
        for (c_size i = 0; i < m_capacity; ++i) {
            if (robinManagers_.at(i).isManaged()) {
                return iterator{&robinManagers_, i};
            }
        }
        return end();
    }

    const_iterator begin() const {
        c_size m_capacity = capacity();
        for (c_size i = 0; i < m_capacity; ++i) {
            if (robinManagers_.at(i).isManaged()) {
                return const_iterator{&robinManagers_, i};
            }
        }
        return end();
    }

    iterator end() {
        return iterator{&robinManagers_, capacity()};
    }

    const_iterator end() const {
        return const_iterator{&robinManagers_, capacity()};
    }

private:
    Array<manager_t> robinManagers_;
};

} // namespace my::util

#endif // HASH_BUCKET_HPP