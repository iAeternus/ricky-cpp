/**
 * @brief 双向链
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef BI_CHAIN_HPP
#define BI_CHAIN_HPP

#include "Chain.hpp"

namespace my::util {

template <BiChainNodeType BiNode>
class BiChainIterator;

/**
 * @brief 双向链
 */
template <BiChainNodeType BiNode, typename Alloc = Allocator<BiNode>>
class BiChain : public Chain<BiNode, Alloc> {
public:
    using Self = BiChain<BiNode, Alloc>;
    using Super = Chain<BiNode, Alloc>;
    friend class BiChainIterator<BiNode>;

    BiChain() :
            Super() {}

    template <typename... Args>
    void append(Args&&... args) {
        BiNode* new_node = alloc_.create(std::forward<Args>(args)...);
        if (Super::size_ == 0) {
            Super::head_ = Super::tail_ = new_node;
        } else {
            Super::tail_->next_ = new_node;
            new_node->prev_ = Super::tail_;
            Super::tail_ = new_node;
        }
        ++Super::size_;
    }

    template <typename... Args>
    void prepend(Args&&... args) {
        BiNode* new_node = alloc_.create(std::forward<Args>(args)...);
        if (Super::size_ == 0) {
            Super::head_ = Super::tail_ = new_node;
        } else {
            new_node->next_ = Super::head_;
            Super::head_->prev_ = new_node;
            Super::head_ = new_node;
        }
        ++Super::size_;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << "<BiChain> [";
        bool first = true;
        for (const auto& current : *this) {
            if (first) {
                first = false;
            } else {
                stream << "<-->";
            }
            stream << current;
        }
        stream << ']';
        return CString{stream.str()};
    }

    using iterator = BiChainIterator<BiNode>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    iterator begin() const {
        return iterator{Super::head_};
    }

    iterator end() const {
        if (!Super::tail_) {
            return iterator{nullptr};
        }
        return iterator{Super::tail_->next_};
    }

    reverse_iterator rbegin() const {
        return reverse_iterator{iterator{Super::tail_}};
    }

    reverse_iterator rend() const {
        return reverse_iterator{iterator{nullptr}};
    }

private:
    Alloc alloc_{};
};

/**
 * @brief 双向链迭代器
 */
template <BiChainNodeType BiNode>
class BiChainIterator : public ChainIterator<BiNode> {
    using Self = BiChainIterator<BiNode>;
    using Super = ChainIterator<BiNode>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename BiNode::value_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    BiChainIterator(BiNode* node = nullptr) :
            Super(node) {}

    BiChainIterator(const Self& other) :
            Super(other) {}

    Self& operator--() {
        if (Super::current_) {
            Super::current_ = Super::current_->prev_;
        }
        return *this;
    }

    Self operator--(i32) {
        Self tmp{Super::current_};
        --tmp;
        return tmp;
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        return Super::current_ - other.current_;
    }

    bool operator==(const Self& other) const {
        return !this->__cmp__(other);
    }

    bool operator!=(const Self& other) const {
        return this->__cmp__(other);
    }
};

template <typename T>
using BiChainList = BiChain<BiChainNode<T>, Allocator<BiChainNode<T>>>;

} // namespace my::util

#endif // BI_CHAIN_HPP