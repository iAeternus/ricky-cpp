/**
 * @brief 字典
 * @author Ricky
 * @date 2024/12/18
 * @version 1.0
 */
#ifndef DICT_HPP
#define DICT_HPP

#include "binary_utils.hpp"
#include "Vec.hpp"
#include "KeyValue.hpp"
#include "HashBucket.hpp"
#include "Pair.hpp"

namespace my::util {

template<typename Alloc>
class BaseString;

/**
 * @class Dict
 * @brief 哈希字典类，提供高效的键值对存储、检索和更新功能
 *
 * Dict 是一个基于哈希表实现的字典容器，支持键值对的快速插入、查询和删除操作
 * 使用分桶和Robin哈希来处理冲突，并通过动态调整桶的大小来保持高效的负载因子
 *
 * @tparam K 键的类型，必须是可哈希的
 * @tparam V 值的类型
 * @tparam Bucket 桶的类型，默认为 RobinHashBucket
 */
template <Hashable K,
          typename V,
          typename Alloc = Allocator<K>,
          typename Bucket = RobinHashBucket<V, typename Alloc::template rebind<RobinManager<V>>::other>>
class Dict : Object<Dict<K, V, Alloc, Bucket>> {
public:
    using key_t = K;         // 键的类型
    using value_t = V;       // 值的类型
    using bucket_t = Bucket; // 桶的类型
    using Self = Dict<key_t, value_t, Alloc, bucket_t>;

    /**
     * @brief 默认构造函数
     * 初始化一个空字典，使用指定的桶大小（默认为 MIN_BUCKET_SIZE）
     * @param bucket_size 桶的初始大小
     */
    Dict(usize bucket_size = MIN_BUCKET_SIZE) :
            bucket_(bucket_size), keys_() {}

    /**
     * @brief 使用初始化列表构造字典
     * @param init_list 初始化列表，包含键值对
     */
    Dict(std::initializer_list<Pair<key_t, value_t>>&& init_list) :
            Dict(roundup2(init_list.size() / MAX_LOAD_FACTOR)) {
        for (auto&& [key, val] : init_list) {
            insert(std::move(key), std::move(val));
        }
    }

    /**
     * @brief 拷贝构造函数
     * @param other 需要拷贝的字典
     */
    Dict(const Self& other) :
            bucket_(other.bucket_), keys_(other.keys_) {}

    /**
     * @brief 移动构造函数
     * @param other 需要移动的字典
     */
    Dict(Self&& other) noexcept :
            bucket_(std::move(other.bucket_)), keys_(std::move(other.keys_)) {}

    /**
     * @brief 拷贝赋值操作符
     * @param other 需要拷贝的字典
     * @return 本字典对象的引用
     */
    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        this->bucket_ = other.bucket_;
        this->keys_ = other.keys_;
        return *this;
    }

    /**
     * @brief 移动赋值操作符
     * @param other 需要移动的字典
     * @return 本字典对象的引用
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        this->bucket_ = std::move(other.bucket_);
        this->keys_ = std::move(other.keys_);
        return *this;
    }

    /**
     * @brief 析构函数
     */
    ~Dict() = default;

    /**
     * @brief 获取字典中键值对的数量
     * @return 返回键值对的数量
     */
    usize size() const {
        return keys_.size();
    }

    /**
     * @brief 判断字典是否为空
     * @return true=是 false=否
     */
    bool empty() const {
        return keys_.empty();
    }

    /**
     * @brief 获取桶的容量
     * @return 返回桶的容量
     */
    usize capacity() const {
        return bucket_.capacity();
    }

    /**
     * @brief 获取负载因子（键值对的数量除以桶的容量）
     * @return 返回负载因子
     */
    f64 load_factor() const {
        return 1.0 * size() / capacity();
    }

    /**
     * @brief 检查字典中是否包含指定的键
     * @param key 需要检查的键
     * @return 如果键存在返回 true，否则返回 false
     */
    bool contains(const key_t& key) const {
        return contains_hash_val(my_hash(key));
    }

    /**
     * @brief 获取键的视图（可迭代范围）
     * @return 返回键的视图
     */
    fn keys() const {
        return std::ranges::subrange(keys_.begin(), keys_.end());
    }

    /**
     * @brief 获取值的视图（可迭代范围）
     * @return 返回值的视图
     */
    fn values() const {
        return std::ranges::subrange(bucket_.begin(), bucket_.end());
    }

    /**
     * @brief 获取指定键对应的值
     * 如果键不存在，抛出 throw not_found_exception
     * @param key 键
     * @return 返回对应值的引用
     */
    template <typename _K>
    value_t& get(const _K& key) {
        auto hash_val = my_hash(key);
        auto* value = get_impl(hash_val);
        if (value == nullptr) {
            throw not_found_exception("key '{}' not found in dict", SRC_LOC, key);
        }
        return *value;
    }

    /**
     * @brief 获取指定键对应的值（常量版本）
     * 如果键不存在，抛出 throw not_found_exception
     * @param key 键
     * @return 返回对应值的常量引用
     */
    template <typename _K>
    const value_t& get(const _K& key) const {
        const auto* value = get_impl(my_hash(key));
        if (value == nullptr) {
            throw not_found_exception("key '{}' not found in dict", SRC_LOC, key);
        }
        return *value;
    }

    /**
     * @brief 获取指定键对应的值或默认值
     * 如果键不存在，返回默认值
     * @param key 键
     * @param default_val 默认值
     * @return 返回对应值的常量引用或默认值
     */
    template <typename _K>
    const value_t& get_or_default(const _K& key, const value_t& default_val) const {
        const auto* value = get_impl(my_hash(key));
        if (value == nullptr) {
            return default_val;
        }
        return *value;
    }

    /**
     * @brief 获取指定键对应的值
     * 如果键不存在，创建键值对，值初始化为对应类型默认值
     * @param key 键
     * @return 返回对应值的可变引用
     */
    template <typename _K>
    value_t& operator[](_K&& key) {
        auto hash_val = my_hash(key);
        if (!contains_hash_val(hash_val)) {
            insert_impl(std::forward<_K>(key), value_t{}, hash_val);
        }
        return *get_impl(hash_val);
    }

    /**
     * @brief 如果键不存在，设置默认值，否则什么都不做
     * @param key 键
     * @param default_val 默认值
     * @return 本字典对象的引用，支持链式编程
     */
    template <typename _V>
    Self& set_default(const key_t& key, _V&& default_val) {
        auto hash_val = my_hash(key);
        if (!contains_hash_val(hash_val)) {
            insert_impl(key, std::forward<_V>(default_val), hash_val);
        }
        return *this;
    }

    /**
     * @brief 向字典中插入键值对，如果键已存在，则覆盖原有值
     * @param key 键
     * @param value 值
     * @return 返回插入或更新后的值的引用
     */
    template <typename _K, typename _V>
    value_t& insert(_K&& key, _V&& value) {
        auto hash_val = my_hash(key);
        auto* m_value = get_impl(hash_val);
        if (m_value) {
            return *m_value = std::forward<_V>(value);
        }
        return insert_impl(std::forward<_K>(key), std::forward<_V>(value), hash_val);
    }

    /**
     * @brief 向字典中插入键值对，如果键已存在，则覆盖原有值（提供哈希值版本）
     * @param key 键
     * @param value 值
     * @param hash_val 键的哈希值
     * @return 返回插入或更新后的值的引用
     */
    template <typename _K, typename _V>
    value_t& insert(_K&& key, _V&& value, hash_t hash_val) {
        auto* m_value = get_impl(hash_val);
        if (m_value) {
            return *m_value = std::forward<_V>(value);
        }
        return insert_impl(std::forward<_K>(key), std::forward<_V>(value), hash_val);
    }

    /**
     * @brief 使用另一个字典更新当前字典
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& update(const Self& other) {
        for (auto&& kv : other) {
            insert(kv.key(), kv.value());
        }
        return *this;
    }

    /**
     * @brief 使用另一个字典更新当前字典（移动语义）
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& update(Self&& other) {
        for (auto& key : other.keys_) {
            auto hash_val = my_hash(key);
            auto& value = *other.get_impl(hash_val);
            insert(std::move(key), std::move(value), hash_val);
        }
        return *this;
    }

    /**
     * @brief 从字典中删除指定的键
     * @param key 键
     */
    void remove(const key_t& key) {
        auto hash_val = my_hash(key);
        bucket_.pop(hash_val);
        keys_.pop(keys_.find(key));
    }

    /**
     * @brief 清空字典
     */
    void clear() {
        bucket_.clear();
        keys_.clear();
    }

    /**
     * @brief 计算两个字典的交集
     * @param other 另一个字典
     * @return 返回两个字典的交集
     */
    Self operator&(const Self& other) const {
        if (this == &other) return *this;
        if (this->size() > other.size()) return other & *this;

        Self res{std::min(this->capacity(), other.capacity())};
        for (auto&& kv : *this) {
            if (other.contains(kv.key())) {
                res.insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
            }
        }
        return res;
    }

    /**
     * @brief 计算两个字典的交集并赋值给当前字典
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator&=(const Self& other) {
        if (this == &other) return *this;
        *this = *this & other;
        return *this;
    }

    /**
     * @brief 计算两个字典的并集，键相同则选择其他字典的值
     * @param other 另一个字典
     * @return 返回两个字典的并集
     */
    Self operator|(const Self& other) const {
        if (this == &other) return *this;
        if (this->size() > other.size()) return other | *this;

        Self res{static_cast<usize>((this->size() + other.size()) / MAX_LOAD_FACTOR)};
        for (auto&& kv : *this) {
            res.insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
        }
        for (auto&& kv : other) {
            res.insert(kv.key(), kv.value());
        }
        return res;
    }

    /**
     * @brief 计算两个字典的并集并赋值给当前字典
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator|=(const Self& other) {
        if (this == &other) return *this;

        for (auto&& kv : other) {
            this->insert(kv.key(), kv.value());
        }
        return *this;
    }

    /**
     * @brief 计算两个字典的并集
     * @param other 另一个字典
     * @return 返回两个字典的并集
     */
    Self operator+(const Self& other) const {
        return *this | other;
    }

    /**
     * @brief 计算两个字典的并集并赋值给当前字典
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator+=(const Self& other) {
        return *this |= other;
    }

    /**
     * @brief 计算两个字典的相对补集
     * @param other 另一个字典
     * @return 返回两个字典的相对补集
     */
    Self operator^(const Self& other) const {
        if (this == &other) return Self{};
        if (this->size() > other.size()) return other ^ *this;

        Self res{std::min(this->capacity(), other.capacity())};
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

    /**
     * @brief 计算两个字典的相对补集并赋值给当前字典
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator^=(const Self& other) {
        if (this == &other) {
            this->clear();
            return *this;
        }
        for (auto&& kv : other) {
            if (this->contains(kv.key())) {
                this->remove(kv.key());
            } else {
                this->insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
            }
        }
        return *this;
    }

    /**
     * @brief 计算两个字典的差集
     * @param other 另一个字典
     * @return 返回两个字典的差集
     */
    Self operator-(const Self& other) const {
        if (this == &other) return Self{};

        Self res{capacity()};
        for (auto&& kv : *this) {
            if (!other.contains(kv.key())) {
                res.insert_impl(kv.key(), kv.value(), my_hash(kv.key()));
            }
        }
        return res;
    }

    /**
     * @brief 计算两个字典的差集并赋值给当前字典
     * @param other 另一个字典
     * @return 本字典对象的引用
     */
    Self& operator-=(const Self& other) {
        if (this == &other) {
            this->clear();
            return *this;
        }
        for (auto&& kv : other) {
            this->remove(kv.key());
        }
        return *this;
    }

    /**
     * @brief 比较两个字典是否相等
     * @param other 另一个字典
     * @return 如果相等返回 true，否则返回 false
     */
    [[nodiscard]] bool __equals__(const Self& other) const {
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

    /**
     * @brief 获取字典的字符串表示
     * @return 返回字典的 CSV 格式的字符串
     */
    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '{';
        for (auto&& item : *this) {
            if constexpr (is_same<key_t, CString, String, std::string>) {
                stream << '\"' << item.key() << '\"';
            } else {
                stream << item.key();
            }
            stream << ':';
            if constexpr (is_same<value_t, CString, String, std::string>) {
                stream << '\"' << item.value() << '\"';
            } else {
                stream << item.value();
            }
            stream << ',';
        }

        auto str = stream.str();
        if (str.size() > 1) {
            str.pop_back();
        }
        str.push_back('}');
        return BaseCString{str};
    }

    /**
     * @class DictIterator
     * @brief 字典迭代器类
     */
    class DictIterator : public Object<DictIterator> {
    public:
        using Self = DictIterator;
        using Super = Object<Self>;

        using iterator_category = std::random_access_iterator_tag;
        using value_type = KeyValueView<key_t, value_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;

        /**
         * @brief 默认构造函数
         */
        DictIterator() :
                dict_(nullptr), index_(0), kv_() {}

        /**
         * @brief 构造一个字典迭代器
         * @param dict 指向字典的指针
         * @param index 初始索引
         */
        DictIterator(const Dict* dict, usize index) :
                dict_(dict), index_(index) {
            update_kv(0);
        }

        /**
         * @brief 拷贝构造函数
         * @param other 需要拷贝的字典迭代器
         */
        DictIterator(const Self& other) = default;

        /**
         * @brief 拷贝赋值操作符
         * @param other 需要拷贝的字典迭代器
         * @return 返回本迭代器对象的引用
         */
        Self& operator=(const Self& other) = default;

        /**
         * @brief 解引用运算符
         * @return 返回当前键值对的引用
         */
        const_reference operator*() const {
            return kv_;
        }

        /**
         * @brief 获取指向当前键值对的指针
         * @return 返回指向当前键值对的指针
         */
        const_pointer operator->() const {
            return &kv_;
        }

        /**
         * @brief 前置自增运算符
         * 移动迭代器到下一个键值对
         * @return 返回自增后的迭代器
         */
        Self& operator++() {
            update_kv(1);
            return *this;
        }

        /**
         * @brief 后置自增运算符
         * 移动迭代器到下一个键值对
         * @return 返回自增前的迭代器
         */
        Self operator++(i32) {
            Self tmp = *this;
            update_kv(1);
            return tmp;
        }

        /**
         * @brief 前置自减运算符
         * 移动迭代器到上一个键值对
         * @return 返回自减后的迭代器
         */
        Self& operator--() {
            update_kv(-1);
            return *this;
        }

        /**
         * @brief 后置自减运算符
         * 移动迭代器到上一个键值对
         * @return 返回自减前的迭代器
         */
        Self operator--(i32) {
            Self tmp = *this;
            update_kv(-1);
            return tmp;
        }

        /**
         * @brief 跳过指定数量的键值对
         * @param n 需要跳过的键值对数量
         * @return 返回自身引用
         */
        Self& operator+=(difference_type n) {
            update_kv(n);
            return *this;
        }

        /**
         * @brief 回退指定数量的键值对
         * @param n 需要回退的键值对数量
         * @return 返回自身引用
         */
        Self& operator-=(difference_type n) {
            update_kv(-n);
            return *this;
        }

        /**
         * @brief 跳过指定数量的键值对并返回新迭代器
         * @param n 需要跳过的键值对数量
         * @return 返回新迭代器
         */
        Self operator+(difference_type n) const {
            return Self(dict_, index_ + n);
        }

        /**
         * @brief 回退指定数量的键值对并返回新迭代器
         * @param n 需要回退的键值对数量
         * @return 返回新迭代器
         */
        Self operator-(difference_type n) const {
            return Self(dict_, index_ - n);
        }

        /**
         * @brief 获取两个迭代器之间的差值
         * @param other 另一个迭代器
         * @return 返回两个迭代器之间的差值
         */
        difference_type operator-(const Self& other) const {
            return index_ - other.index_;
        }

        /**
         * @brief 比较两个迭代器是否相等
         * @param other 另一个迭代器
         * @return 如果相等返回 true，否则返回 false
         */
        [[nodiscard]] bool __equals__(const Self& other) const {
            return dict_ == other.dict_ && index_ == other.index_;
        }

        /**
         * @brief 比较两个迭代器是否相等
         * @param other 另一个迭代器
         * @return 如果相等返回 true，否则返回 false
         */
        bool operator==(const Self& other) const {
            return this->__equals__(other);
        }

        /**
         * @brief 比较两个迭代器是否不相等
         * @param other 另一个迭代器
         * @return 如果不相等返回 true，否则返回 false
         */
        bool operator!=(const Self& other) const {
            return !this->__equals__(other);
        }

    private:
        /**
         * @brief 更新当前键值对
         * @param add 需要调整的指数
         */
        void update_kv(i64 add) {
            index_ += add;
            if (index_ < 0 || index_ >= dict_->size()) {
                kv_.set(nullptr, nullptr);
                return;
            }
            const key_t& key = dict_->keys_[index_];
            kv_.set(&key, &dict_->get(key));
        }

    private:
        const Dict* dict_; // 指向字典的指针
        usize index_;      // 当前迭代器所在索引
        value_type kv_;    // 当前键值对
    };

    using iterator = DictIterator;       // 字典迭代器类型
    using const_iterator = DictIterator; // 常量字典迭代器类型

    /**
     * @brief 获取字典的起始迭代器
     * @return 返回起始迭代器
     */
    iterator begin() {
        return iterator{this, 0LL};
    }

    /**
     * @brief 获取字典的起始迭代器（常量版本）
     * @return 返回常量起始迭代器
     */
    const_iterator begin() const {
        return const_iterator{this, 0LL};
    }

    /**
     * @brief 获取字典的末尾迭代器
     * @return 返回末尾迭代器
     */
    iterator end() {
        return iterator{this, size()};
    }

    /**
     * @brief 获取字典的末尾迭代器（常量版本）
     * @return 返回常量末尾迭代器
     */
    const_iterator end() const {
        return const_iterator{this, size()};
    }

private:
    /**
     * @brief 检查哈希值对应的键值对是否存在
     * @param hash_val 哈希值
     * @return 如果存在返回 true，否则返回 false
     */
    bool contains_hash_val(hash_t hash_val) const {
        return bucket_.contains(hash_val);
    }

    /**
     * @brief 扩展桶的大小
     * @details
     * 当字典的负载因子（键值对的数量除以桶的容量）超过最大阈值 `MAX_LOAD_FACTOR` 时，
     * 调用此函数将桶的大小扩大一倍桶的初始大小为 `MIN_BUCKET_SIZE`，新的桶大小为当前大小的两倍
     */
    void expand() {
        bucket_.expand(std::max<usize>(MIN_BUCKET_SIZE, capacity() << 1LL));
    }

    /**
     * @brief 向字典中插入键值对
     * 不检查键是否存在
     * @tparam _K 键的类型
     * @tparam _V 值的类型
     * @param key 键
     * @param value 值
     * @param hash_val 哈希值
     * @return 返回插入值的引用
     */
    template <typename _K, typename _V>
    value_t& insert_impl(_K&& key, _V&& value, hash_t hash_val) {
        if (load_factor() >= MAX_LOAD_FACTOR) {
            expand();
        }

        value_t& v = *bucket_.set_value(std::forward<_V>(value), hash_val);
        keys_.append(std::forward<_K>(key));
        return v;
    }

    /**
     * @brief 向字典中插入键值对
     * 不检查键是否存在
     * @tparam _K 键的类型
     * @tparam _V 值的类型
     * @param key 键
     * @param value 值
     * @return 返回插入值的引用
     */
    template <typename _K, typename _V>
    value_t& insert_impl(_K&& key, _V&& value) {
        return insert_impl(std::forward<_K>(key), std::forward<_V>(value), my_hash(static_cast<const key_t&>(key)));
    }

    /**
     * @brief 根据哈希值获取值
     * @param hash_val 哈希值
     * @return 返回值的指针
     */
    value_t* get_impl(hash_t hash_val) {
        return bucket_.try_get(hash_val);
    }

    /**
     * @brief 根据哈希值获取值（常量版本）
     * @param hash_val 哈希值
     * @return 返回值的常量指针
     */
    const value_t* get_impl(hash_t hash_val) const {
        return bucket_.try_get(hash_val);
    }

private:
    bucket_t bucket_;        // 桶对象
    Vec<key_t, Alloc> keys_; // 键的动态数组

    constexpr static f64 MAX_LOAD_FACTOR = 0.75; // 最大负载因子
    constexpr static usize MIN_BUCKET_SIZE = 8;  // 最小桶大小
};

} // namespace my::util

#endif // DICT_HPP