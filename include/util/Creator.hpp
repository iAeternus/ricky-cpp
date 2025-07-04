/**
 * @brief 自动管理对象创建
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CREATOR_HPP
#define CREATOR_HPP

#include "Allocator.hpp"
#include "NoCopy.hpp"
#include "Object.hpp"

namespace my::util {

template <typename T, typename Alloc = Allocator<T>>
class Creator : public Object<Creator<T, Alloc>>, public NoCopy {
public:
    using value_t = T;
    using Self = Creator<value_t, Alloc>;

    /**
     * @brief 安全创建对象，返回构造好的指针
     * @param args 构造参数
     * @return 指向新对象的指针，若失败返回 nullptr
     */
    template <typename... Args>
    fn operator()(Args&&... args)->value_t* {
        auto* ptr = alloc_.allocate(1);
        if (!ptr) return nullptr;

        try {
            alloc_.construct(ptr, std::forward<Args>(args)...);
        } catch (...) {
            alloc_.destroy(ptr);
            throw;
        }

        return ptr;
    }

private:
    Alloc alloc_{};
};

} // namespace my::util

#endif // CREATOR_HPP