/**
 * @brief 字典
 * @author Ricky
 * @date 2024/12/18
 * @version 1.0
 */
#ifndef DICT_HPP
#define DICT_HPP

#include "ricky_memory.hpp"
#include "binary_utils.hpp"
#include "HashBucket.hpp"
#include "RelationIterator.hpp"

namespace my::util {

class String;

/**
 * @brief 键值对视图
 */
template <Hashable K, typename V>
class KeyValueView : Object<KeyValueView<K, V>> {
    using self = KeyValueView<K, V>;

public:
    using key_t = K;
    using value_t = V;

    KeyValueView(const key_t* key = nullptr, const value_t* value = nullptr) :
            key_(key), value_(value) {}

    KeyValueView(const self& other) = default;

    self& operator=(const self& other) = default;

    self& set(const key_t* key, const value_t* value) {
        this->key_ = key;
        this->value_ = value;
        return *this;
    }

    const key_t& key() const {
        if (key_ == nullptr) {
            KeyError("key is null");
        }
        return *key_;
    }

    const value_t& value() const {
        if (value_ == nullptr) {
            ValueError("value is null");
        }
        return *value_;
    }

private:
    const key_t* key_;
    const value_t* value_;
};

/**
 * @brief 字典
 */
template <Hashable K, typename V, typename BUCKET = RobinHashBucket<V>>
class Dict : Object<Dict<K, V, BUCKET>> {
    using self = Dict<K, V, BUCKET>;

public:
    using key_t = K;
    using value_t = V;
    using bucket_t = BUCKET;

    Dict(c_size bucketSize = MIN_BUCKET_SIZE) :
            bucket_(bucketSize), keys_() {}

    Dict(std::initializer_list<std::pair<key_t, value_t>>&& initList) :
            Dict(roundup2(initList.size() / MAX_LOAD_FACTOR)) {
        for (auto&& kv : initList) {
            insert(kv.first, kv.second);
        }
    }

    Dict(const self& other) :
            bucket_(other.bucket_), keys_(other.keys_) {}

    Dict(self&& other) noexcept :
            bucket_(std::move(other.bucket_)), keys_(std::move(other.keys_)) {}

    self& operator=(const self& other) {
        if (this == &other) return *this;

        this->bucket_ = other.bucket_;
        this->keys_ = other.keys_;
        return *this;
    }

    self& operator=(self&& other) noexcept {
        if (this == &other) return *this;

        this->bucket_ = std::move(other.bucket_);
        this->keys_ = std::move(other.keys_);
        return *this;
    }

    ~Dict() = default;

    /**
     * @brief key-value对的数量
     */
    c_size size() const {
        return keys_.size();
    }

    /**
     * @brief 桶容量
     */
    c_size capacity() const {
        return bucket_.capacity();
    }

    /**
     * @brief 负载因子
     */
    double loadFactor() const {
        return 1.0 * size() / capacity();
    }

    /**
     * @brief 判断key是否存在
     * @return true=是 false=否
     */
    bool contains(const key_t& key) const {
        return contains_hash_val(my_hash(key));
    }

    /**
     * @brief key的迭代视图
     */
    auto keys() const {
        return std::ranges::subrange(keys_.begin(), keys_.end());
    }

    /**
     * @brief value的迭代视图
     */
    auto values() const {
        return std::ranges::subrange(bucket_.begin(), bucket_.end());
    }

    /**
     * @brief 获得key对应的value，若key不存在，则抛出KeyError
     */
    value_t& get(const key_t& key) {
        hash_t hashVal = my_hash(key);
        auto* value = get_impl(hashVal);
        if (value == nullptr) {
            KeyError(std::format("Key '{}' not found in dict", key));
            // return None<value_t>;
        }
        return *value;
    }

    const value_t& get(const key_t& key) const {
        const auto* value = get_impl(my_hash(key));
        if (value == nullptr) {
            KeyError(std::format("Key '{}' not found in dict", key));
            // return None<value_t>;
        }
        return *value;
    }

    /**
     * @brief 重载[]运算符，key 必须存在，否则KeyError
     */
    const value_t& operator[](const key_t& key) const {
        return get(key);
    }

    /**
     * @brief 获得key对应的value，若key不存在，则返回defaultValue
     */
    value_t& getOrDefault(const key_t& key, value_t& defaultValue) {
        auto* value = get_impl(my_hash(key));
        if (value == nullptr) {
            return defaultValue;
        }
        return *value;
    }

    const value_t& getOrDefault(const key_t& key, const value_t& defaultValue) const {
        const auto* value = get_impl(my_hash(key));
        if (value == nullptr) {
            return defaultValue;
        }
        return *value;
    }

    /**
     * @brief 重载[]运算符，若key不存在，则创建并返回一个默认值
     */
    value_t& operator[](const key_t& key) {
        hash_t hashVal = my_hash(key);
        if (!contains_hash_val(hashVal)) {
            insert_impl(key, value_t{}, hashVal);
        }
        return *get_impl(hashVal);
    }

    /**
     * @brief 若key不存在，则添加一个默认值，反之则什么都不做
     */
    template <typename _V>
    self& setdefault(const key_t& key, _V&& defaultValue) {
        hash_t hashVal = my_hash(key);
        if (!contains_hash_val(hashVal)) {
            insert_impl(key, std::forward<_V>(defaultValue), hashVal);
        }
        return *this;
    }

    /**
     * @brief 向字典中插入一个key-value对，若key已经存在，则覆盖原有值
     */
    template <typename _K, typename _V>
    value_t& insert(_K&& key, _V&& value) {
        hash_t hashVal = my_hash(key);
        auto* m_value = get_impl(hashVal);
        if (m_value) {
            return *m_value = std::forward<_V>(value);
        }
        return insert_impl(std::forward<_K>(key), std::forward<_V>(value), hashVal);
    }

    template <typename _K, typename _V>
    value_t& insert(_K&& key, _V&& value, hash_t hashVal) {
        auto* m_value = get_impl(hashVal);
        if (m_value) {
            return *m_value = std::forward<_V>(value);
        }
        return insert_impl(std::forward<_K>(key), std::forward<_V>(value), hashVal);
    }

    /**
     * @brief 根据传入的字典更新字典
     */
    self& update(const self& other) {
        for (auto&& kv : other) {
            insert(kv.key(), kv.value());
        }
        return *this;
    }

    self& update(self&& other) {
        for (auto& key : other.keys_) {
            hash_t hashVal = my_hash(key);
            auto& value = *other.get_impl(hashVal);
            insert(std::move(key), std::move(value), hashVal);
        }
        return *this;
    }

    void pop(const key_t& key) {
        hash_t hashVal = my_hash(key);
        bucket_.pop(hashVal);
        keys_.pop(keys_.find(key));
    }

    void clear() {
        bucket_.clear();
        keys_.clear();
    }

    /**
     * @brief 交集
     */
    self operator&(const self& other) const {
        if (this == &other) return *this;
        if (this->size() > other.size()) return other & *this;

        self res{std::min(this->capacity(), other.capacity())};
        for (auto&& kv : *this) {
            if (other.contains(kv.key())) {
                res.insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
            }
        }
        return res;
    }

    self& operator&=(const self& other) {
        if (this == &other) return *this;
        *this = *this & other;
        return *this;
    }

    /**
     * @brief 并集，键相同默认选择other的值
     */
    self operator|(const self& other) const {
        if (this == &other) return *this;
        if (this->size() > other.size()) return other | *this;

        self res{static_cast<c_size>((this->size() + other.size()) / MAX_LOAD_FACTOR)};
        for (auto&& kv : *this) {
            res.insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
        }
        for (auto&& kv : other) {
            res.insert(kv.key(), kv.value());
        }
        return res;
    }

    self& operator|=(const self& other) {
        if (this == &other) return *this;

        for (auto&& kv : other) {
            this->insert(kv.key(), kv.value());
        }
        return *this;
    }

    self operator+(const self& other) const {
        return *this | other;
    }

    self& operator+=(const self& other) {
        return *this |= other;
    }

    /**
     * @brief 相对补集，并集与交集的差集
     */
    self operator^(const self& other) const {
        if (this == &other) return self{};
        if (this->size() > other.size()) return other ^ *this;

        self res{std::min(this->capacity(), other.capacity())};
        for (auto&& kv : *this) {
            if (!other.contains(kv.key())) {
                res.insert(kv.key(), kv.value());
            }
        }
        for (auto&& kv : other) {
            if (!this->contains(kv.key())) {
                res.insert(kv.key(), kv.value());
            }
        }
        return res;
    }

    self& operator^=(const self& other) {
        if (this == &other) {
            this->clear();
            return *this;
        }
        for (auto&& kv : other) {
            if (this->contains(kv.key())) {
                this->pop(kv.key());
            } else {
                this->insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
            }
        }
        return *this;
    }

    /**
     * @brief 差集
     */
    self operator-(const self& other) const {
        if (this == &other) return self{};

        self res{capacity()};
        for (auto&& kv : *this) {
            if (!other.contains(kv.key())) {
                res.insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
            }
        }
        return res;
    }

    self& operator-=(const self& other) {
        if (this == &other) {
            this->clear();
            return *this;
        }
        for (auto&& kv : other) {
            this->pop(kv.key());
        }
        return *this;
    }

    bool __equals__(const self& other) const {
        if (this->size() != other.size()) return false;

        for (auto&& kv : *this) {
            if (!other.contains(kv.key())) {
                return false;
            }
            if (kv.value() != other.get(kv.key())) {
                return false;
            }
        }
        return true;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << '{';
        for (auto&& item : *this) {
            if constexpr (is_same<key_t, CString, String, std::string>) {
                stream << '\"' << item.key() << '\"';
            } else {
                stream << item.key();
            }
            stream << ": ";
            if constexpr (is_same<value_t, CString, String, std::string>) {
                stream << '\"' << item.value() << '\"';
            } else {
                stream << item.value();
            }
            stream << ", ";
        }

        std::string str = stream.str();
        if (str.size() > 2) {
            str.pop_back();
            str.pop_back();
        }
        str.push_back('}');
        return CString{str};
    }

    class DictIterator : public Object<DictIterator> {
        using self = DictIterator;
        using super = Object<self>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = KeyValueView<key_t, value_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        DictIterator() :
                dict_(nullptr), index_(0), kv_() {}

        DictIterator(const Dict* dict, c_size index) :
                dict_(dict), index_(index) {
            update_kv(0);
        }

        DictIterator(const self& other) = default;

        self& operator=(const self& other) = default;

        const value_type& operator*() const {
            return kv_;
        }

        const value_type* operator->() const {
            return &kv_;
        }

        self& operator++() {
            update_kv(1);
            return *this;
        }

        self operator++(int) {
            self tmp = *this;
            update_kv(1);
            return tmp;
        }

        self& operator--() {
            update_kv(-1);
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            update_kv(-1);
            return tmp;
        }

        self& operator+=(difference_type n) {
            update_kv(n);
            return *this;
        }

        self& operator-=(difference_type n) {
            update_kv(-n);
            return *this;
        }

        self operator+(difference_type n) const {
            return self(dict_, index_ + n);
        }

        self operator-(difference_type n) const {
            return self(dict_, index_ - n);
        }

        difference_type operator-(const self& other) const {
            return index_ - other.index_;
        }

        bool __equals__(const self& other) const {
            return dict_ == other.dict_ && index_ == other.index_;
        }

        bool operator==(const self& other) const {
            return this->__equals__(other);
        }

        bool operator!=(const self& other) const {
            return !this->__equals__(other);
        }

    private:
        void update_kv(c_size add) {
            index_ += add;
            if (index_ < 0 || index_ >= dict_->size()) {
                kv_.set(nullptr, nullptr);
                return;
            }
            const key_t& key = dict_->keys_[index_];
            kv_.set(&key, &dict_->get(key));
        }

    private:
        const Dict* dict_;
        c_size index_; // 键视图索引
        value_type kv_;
    };

    using iterator = DictIterator;
    using const_iterator = DictIterator;

    iterator begin() {
        return iterator{this, 0LL};
    }

    const_iterator begin() const {
        return const_iterator{this, 0LL};
    }

    iterator end() {
        return iterator{this, size()};
    }

    const_iterator end() const {
        return const_iterator{this, size()};
    }

private:
    bool contains_hash_val(hash_t hashVal) const {
        return bucket_.contains(hashVal);
    }

    void expand() {
        bucket_.expand(std::max<c_size>(MIN_BUCKET_SIZE, capacity() << 1LL));
    }

    /**
     * @brief 向字典中插入一个key-value对。不检查key是否存在
     */
    template <typename _K, typename _V>
    value_t& insert_impl(_K&& key, _V&& value, hash_t hashVal) {
        if (loadFactor() >= MAX_LOAD_FACTOR) {
            expand();
        }

        value_t& v = *bucket_.setValue(std::forward<_V>(value), hashVal);
        keys_.append(std::forward<_K>(key));
        return v;
    }

    template <typename _K, typename _V>
    value_t& insert_impl(_K&& key, _V&& value) {
        return insert_impl(std::forward<_K>(key), std::forward<_V>(value), my_hash(static_cast<const key_t&>(key)));
    }

    /**
     * @brief 获得hash值对应的value指针。不检查key是否存在
     */
    value_t* get_impl(hash_t hashVal) {
        return bucket_.tryGet(hashVal);
    }

    const value_t* get_impl(hash_t hashVal) const {
        return bucket_.tryGet(hashVal);
    }

private:
    bucket_t bucket_;
    DynArray<key_t> keys_;

    constexpr static double MAX_LOAD_FACTOR = 0.75;
    constexpr static c_size MIN_BUCKET_SIZE = 8;
};

} // namespace my::util

#endif // DICT_HPP