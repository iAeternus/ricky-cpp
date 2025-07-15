/**
 * @brief 键值对
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef KEY_VALUE_HPP
#define KEY_VALUE_HPP

#include "Exception.hpp"

namespace my::util {

/**
 * @brief 键值对视图
 */
template <KeyType K, typename V>
class KeyValueView : Object<KeyValueView<K, V>> {
public:
    using key_t = K;
    using value_t = V;
    using Self = KeyValueView<key_t, value_t>;

    KeyValueView(const key_t* key = nullptr, const value_t* value = nullptr) :
            key_(key), value_(value) {}

    KeyValueView(const Self& other) = default;

    Self& operator=(const Self& other) = default;

    Self& set(const key_t* key, const value_t* value) {
        this->key_ = key;
        this->value_ = value;
        return *this;
    }

    const key_t& key() const {
        if (key_ == nullptr) {
            throw null_pointer_exception("key is null");
        }
        return *key_;
    }

    const value_t& value() const {
        if (value_ == nullptr) {
            throw null_pointer_exception("value is null");
        }
        return *value_;
    }

    template <std::size_t I>
    decltype(auto) get() & {
        if constexpr (I == 0) {
            return *key_;
        } else if constexpr (I == 1) {
            return *value_;
        }
    }

    template <std::size_t I>
    decltype(auto) get() const& {
        if constexpr (I == 0) {
            return *key_;
        } else if constexpr (I == 1) {
            return *value_;
        }
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << *key_ << "->" << *value_;
        return CString{stream.str()};
    }

private:
    const key_t* key_;
    const value_t* value_;
};

} // namespace my::util

/**
 * @brief 结构化绑定支持
 */
namespace std {

template <my::KeyType K, typename V>
struct tuple_size<my::util::KeyValueView<K, V>>
        : integral_constant<size_t, 2> {};

template <size_t I, my::KeyType K, typename V>
struct tuple_element<I, my::util::KeyValueView<K, V>> {
    static_assert(I < 2, "Index out of bounds for KeyValueView");
    using type = conditional_t<I == 0, K, V>;
};

} // namespace std

namespace my {

// 基于索引的get函数
template <size_t I, my::KeyType K, typename V>
auto& get(util::KeyValueView<K, V>& kv) {
    static_assert(I < 2, "KeyValueView index out of range");
    if constexpr (I == 0) {
        return kv.key();
    } else {
        return kv.value();
    }
}

template <size_t I, my::KeyType K, typename V>
const auto& get(util::KeyValueView<K, V>& kv) {
    static_assert(I < 2, "KeyValueView index out of range");
    if constexpr (I == 0) {
        return kv.key();
    } else {
        return kv.value();
    }
}

} // namespace my

#endif // KEY_VALUE_HPP