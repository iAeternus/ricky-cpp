/**
 * @brief 自动管理对象创建
 * @author Ricky
 * @date 2024/12/14
 * @version 1.0
 */
#ifndef CREATOR_HPP
#define CREATOR_HPP

#include "ricky_memory.hpp"
#include "DynArray.hpp"

namespace my::util {

template <typename T>
class Creator : public Object<Creator<T>> {
    using self = Creator<T>;
    using super = Object<self>;

public:
    Creator() = default;

    Creator(const self& other) = delete;

    self& operator=(const self& other) = delete;

    Creator(self&& other) noexcept :
            createdValues_(std::move(other.createdValues_)) {}

    self& operator=(self&& other) noexcept {
        this->createdValues_ = std::move(other.createdValues_);
        return *this;
    }

    template <typename... Args>
    T* operator()(Args&&... args) {
        return &createdValues_.append(T{std::forward<Args>(args)...});
    }

private:
    DynArray<T> createdValues_;
};

} // namespace my::util

#endif // CREATOR_HPP