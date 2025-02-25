/**
 * @brief 链节点
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CHAIN_NODE_HPP
#define CHAIN_NODE_HPP

#include "raise_error.hpp"

namespace my::util {

/**
 * @brief 链节点类型约束
 */
template <typename T>
concept ChainNodeType = requires(T a, const T& b, T&& c) {
    { T() }
    ->std::same_as<T>;
    { a = b }
    ->std::same_as<T&>;
    { T(b) }
    ->std::same_as<T>;
    { a = std::move(c) }
    ->std::same_as<T&>;
    { T(std::move(c)) }
    ->std::same_as<T>;
    {a.next_};
    {a.value_};
};

/**
 * @brief 双向链节点类型约束
 */
template <typename T>
concept BiChainNodeType = ChainNodeType<T>&& requires(T a) {
    {a.prev_};
};

/**
 * @brief 链节点，只能前向移动
 */
template <typename T>
class ChainNode : public Object<ChainNode<T>> {
    using self = ChainNode<T>;
    using super = Object<self>;

public:
    using value_t = T;

    ChainNode(const value_t& value = {}, self* next = nullptr) :
            value_(value), next_(next) {}

    ChainNode(const self& other) :
            value_(other.value_), next_(other.next_) {}

    ChainNode(self&& other) noexcept :
            value_(std::move(other.value_)), next_(other.next_) {
        other.next_ = nullptr;
    }

    self& operator=(const self& other) {
        this->value_ = other.value_;
        this->next_ = nullptr;
        return *this;
    }

    self& operator=(self&& other) noexcept {
        this->value_ = std::move(other.value_);
        this->next_ = other.next_;
        other.next_ = nullptr;
        return *this;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << "<Node  " << value_ << '>';
        return CString(stream.str());
    }

    bool __equals__(const self& other) const {
        return this->value_ == other.value_;
    }

    bool operator==(const self& other) const {
        return this->__equals__(other);
    }

    bool operator!=(const self& other) const {
        return !this->__equals__(other);
    }

    value_t value_;
    self* next_;
};

/**
 * @brief 双向链节点，可以前后移动
 */
template <typename T>
class BiChainNode : public Object<BiChainNode<T>> {
    using self = BiChainNode<T>;
    using super = Object<self>;

public:
    using value_t = T;

    BiChainNode(const value_t& value = {}) :
            value_(value), next_(nullptr), prev_(nullptr) {}

    BiChainNode(const self& other) :
            value_(other.value_), next_(nullptr), prev_(nullptr) {}

    BiChainNode(self&& other) noexcept :
            value_(std::move(other.value_)), next_(other.next_), prev_(other.prev_) {
        other.next_ = other.prev_ = nullptr;
    }

    self& operator=(const self& other) {
        this->value_ = other.value_;
        this->next_ = this->prev_ = nullptr;
        return *this;
    }

    self& operator=(self&& other) noexcept {
        this->value_ = std::move(other.value_);
        this->next_ = other.next_;
        this->prev_ = other.prev_;
        other.next_ = other.prev_ = nullptr;
        return *this;
    }

    CString __str__() const {
        std::stringstream stream;
        stream << "<BiNode  " << value_ << ">";
        return CString{stream.str()};
    }

    bool __equals__(const self& other) const {
        return this->value_ == other.value_;
    }

    bool operator==(const self& other) const {
        return this->__equals__(other);
    }

    bool operator!=(const self& other) const {
        return !this->__equals__(other);
    }

    value_t value_;
    self *next_, *prev_;
};

} // namespace my::util

#endif // CHAIN_NODE_HPP