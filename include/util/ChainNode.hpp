/**
 * @brief 链节点
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CHAIN_NODE_HPP
#define CHAIN_NODE_HPP

#include "Object.hpp"

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
public:
    using value_t = T;
    using Self = ChainNode<value_t>;
    using Super = Object<Self>;

    ChainNode(const value_t& value = {}, Self* next = nullptr) :
            value_(value), next_(next) {}

    ChainNode(const Self& other) :
            value_(other.value_), next_(other.next_) {}

    ChainNode(Self&& other) noexcept :
            value_(std::move(other.value_)), next_(other.next_) {
        other.next_ = nullptr;
    }

    Self& operator=(const Self& other) {
        this->value_ = other.value_;
        this->next_ = nullptr;
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        this->value_ = std::move(other.value_);
        this->next_ = other.next_;
        other.next_ = nullptr;
        return *this;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << "<Node  " << value_ << '>';
        return CString(stream.str());
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return this->value_ == other.value_;
    }

    bool operator==(const Self& other) const {
        return this->__equals__(other);
    }

    bool operator!=(const Self& other) const {
        return !this->__equals__(other);
    }

    value_t value_;
    Self* next_;
};

/**
 * @brief 双向链节点，可以前后移动
 */
template <typename T>
class BiChainNode : public Object<BiChainNode<T>> {
public:
    using value_t = T;
    using Self = BiChainNode<value_t>;
    using Super = Object<Self>;

    BiChainNode(const value_t& value = {}) :
            value_(value), next_(nullptr), prev_(nullptr) {}

    BiChainNode(const Self& other) :
            value_(other.value_), next_(nullptr), prev_(nullptr) {}

    BiChainNode(Self&& other) noexcept :
            value_(std::move(other.value_)), next_(other.next_), prev_(other.prev_) {
        other.next_ = other.prev_ = nullptr;
    }

    Self& operator=(const Self& other) {
        this->value_ = other.value_;
        this->next_ = this->prev_ = nullptr;
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        this->value_ = std::move(other.value_);
        this->next_ = other.next_;
        this->prev_ = other.prev_;
        other.next_ = other.prev_ = nullptr;
        return *this;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << "<BiNode  " << value_ << ">";
        return CString{stream.str()};
    }

    [[nodiscard]] bool __equals__(const Self& other) const {
        return this->value_ == other.value_;
    }

    bool operator==(const Self& other) const {
        return this->__equals__(other);
    }

    bool operator!=(const Self& other) const {
        return !this->__equals__(other);
    }

    value_t value_;
    Self *next_, *prev_;
};

} // namespace my::util

#endif // CHAIN_NODE_HPP