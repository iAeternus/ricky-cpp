/**
 * @brief 键值对
 * @author Ricky
 * @date 2025/1/13
 * @version 1.0
 */
#ifndef KEY_VALUE_HPP
#define KEY_VALUE_HPP

#include "ricky_concepts.hpp"

namespace my::util {

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

} // namespace my::util

#endif // KEY_VALUE_HPP