#ifndef BI_CHAIN_HPP
#define BI_CHAIN_HPP

#include "Chain.hpp"

namespace my::util {

/**
 * @brief 双向链
 */
template <BiChainNodeType Node, typename C = Creator<Node>>
class BiChain : public Chain<Node, C> {
    using self = BiChain<Node, C>;
    using super = Chain<Node, C>;

public:
};

} // namespace my::util

#endif // BI_CHAIN_HPP