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
template <BiChainNodeType BiNode, typename C = Creator<BiNode>>
class BiChain : public Chain<BiNode, C> {
    using self = BiChain<BiNode, C>;
    using super = Chain<BiNode, C>;

public:
    friend class BiChainIterator<BiNode>;

    BiChain() :
            super() {}

    template <typename... Args>
    void append(Args&&... args) {
        BiNode* newNode = super::creator_(std::forward<Args>(args)...);

        if (super::size_ == 0) {
            super::head_ = newNode;
        } else {
            super::tail_->next_ = newNode;
            newNode->prev_ = super::tail_;
        }
        super::tail_ = newNode;
        ++super::size_;
    }

    template <typename... Args>
    void prepend(Args&&... args) {
        BiNode* newNode = super::creator_(std::forward<Args>(args)...);

        if (super::size_ == 0) {
            super::tail_ = newNode;
        } else {
            super::head_->prev_ = newNode;
            newNode->next_ = super::head_;
        }
        super::head_ = newNode;
        ++super::size_;
    }

    CString __str__() const {
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
        return CString(stream.str());
    }

    using iterator = BiChainIterator<BiNode>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    iterator begin() const {
        return iterator{super::head_};
    }

    iterator end() const {
        if(!super::tail_) {
            return iterator{nullptr};
        }
        return iterator{super::tail_->next_};
    }

    reverse_iterator rbegin() const {
        return reverse_iterator{super::tail_};
    }

    reverse_iterator rend() const {
        if(!super::head_) {
            return reverse_iterator{nullptr};
        }
        return reverse_iterator{super::head_->prev_};
    }
};

/**
 * @brief 双向链迭代器
 */
template <BiChainNodeType BiNode>
class BiChainIterator : public ChainIterator<BiNode> {
    using self = BiChainIterator<BiNode>;
    using super = ChainIterator<BiNode>;

public:
    // using iterator_category = std::bidirectional_iterator_tag;
    // using value_type = typename BiNode::value_t;
    // using difference_type = std::ptrdiff_t;
    // using pointer = value_type*;
    // using const_pointer = const value_type*;
    // using reference = value_type&;
    // using const_reference = const value_type&;

    BiChainIterator(BiNode* node = nullptr) :
            super(node) {}

    BiChainIterator(const self& other) :
            super(other) {}

    self& operator--() {
        super::current_ = super::current_->prev_;
        return *this;
    }

    self operator--(int) {
        self tmp{super::current_};
        super::current_ = super::current_->prev_;
        return tmp;
    }

    cmp_t __cmp__(const self& other) const {
        return super::current_ - other.current_;
    }

    bool operator==(const self& other) const {
        return !this->__cmp__(other);
    }

    bool operator!=(const self& other) const {
        return this->__cmp__(other);
    }

};

template <typename T>
using BiChainList = BiChain<BiChainNode<T>, Creator<BiChainNode<T>>>;

} // namespace my::util

#endif // BI_CHAIN_HPP