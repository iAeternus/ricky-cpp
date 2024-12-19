/**
 * @brief 禁止拷贝和移动
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef NO_COPY_HPP
#define NO_COPY_HPP

namespace my {

struct NoCopy {
    using self = NoCopy;

    NoCopy() = default;

    NoCopy(const self& other) = delete;

    NoCopy(self&& other) noexcept = delete;

    self& operator=(const self& other) = delete;

    self& operator=(self& other) noexcept = delete;

    ~NoCopy() = default;
};

} // namespace my

#endif // NO_COPY_HPP