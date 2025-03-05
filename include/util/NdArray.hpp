/**
 * @brief 多维数组
 * @author Ricky
 * @date 2025/2/28
 * @version 1.0
 */
#ifndef ND_ARRAY_HPP
#define ND_ARRAY_HPP

#include "Vector.hpp"

namespace my::util {

template<typename T>
class NdArray : public Object<NdArray<T>> {
public:
    using self = NdArray<T>;
    using value_t = T;
private:
    
};

} // namespace my::util

#endif // ND_ARRAY_HPP