/**
 * @brief 自动管理对象创建
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CREATOR_HPP
#define CREATOR_HPP

#include "ricky_memory.hpp"
#include "NoCopy.hpp"
#include "Object.hpp"

namespace my::util {

template <typename T>
class Creator : public Object<Creator<T>>, public NoCopy {
public:
    using value_t = T;
    using Self = Creator<value_t>;

    /**
     * @brief 安全创建对象，返回构造好的指针
     * @param args 构造参数
     * @return 指向新对象的指针，若失败返回 nullptr
     */
    template <typename... Args>
    fn operator()(Args&&... args)->value_t* {
        auto* ptr = my_alloc<value_t>(1);
        if (!ptr) return nullptr;

        try {
            my_construct(ptr, std::forward<Args>(args)...);
        } catch (...) {
            my_delloc(ptr);
            throw;
        }

        return ptr;
    }
};

} // namespace my::util

#endif // CREATOR_HPP