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
    using Self = NoCopy;

    NoCopy() = default;

    NoCopy(const Self& other) = delete;

    NoCopy(Self&& other) noexcept = delete;

    Self& operator=(const Self& other) = delete;

    Self& operator=(Self& other) noexcept = delete;

    ~NoCopy() = default;
};

} // namespace my

#endif // NO_COPY_HPP