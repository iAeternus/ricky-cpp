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
public:
    using value_t = T;
    using Self = Creator<value_t>;
    using Super = Object<Self>;

    Creator() = default;

    Creator(const Self& other) = delete;

    Self& operator=(const Self& other) = delete;

    Creator(Self&& other) noexcept :
            created_values_(std::move(other.created_values_)) {}

    Self& operator=(Self&& other) noexcept {
        this->created_values_ = std::move(other.created_values_);
        return *this;
    }

    template <typename... Args>
    value_t* operator()(Args&&... args) {
        return &created_values_.append(value_t{std::forward<Args>(args)...});
    }

private:
    DynArray<value_t> created_values_;
};

} // namespace my::util

#endif // CREATOR_HPP