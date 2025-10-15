/**
 * @file NoCopy.hpp
 * @brief 禁止拷贝的基类
 * @author Ricky
 * @date 2024/12/19
 * @version 1.1
 */
#ifndef NO_COPY_HPP
#define NO_COPY_HPP

namespace my {

/**
 * @class NoCopy
 * @brief 禁止拷贝，允许移动
 */
class NoCopy {
protected:
    using Self = NoCopy;

    constexpr NoCopy() = default;
    ~NoCopy() = default;

    NoCopy(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    NoCopy(Self&&) noexcept = default;
    Self& operator=(Self&&) noexcept = default;
};

/**
 * @class NoMove
 * @brief 禁止移动，允许拷贝
 */
class NoMove {
protected:
    using Self = NoMove;

    constexpr NoMove() = default;
    ~NoMove() = default;

    NoMove(const Self&) = default;
    Self& operator=(const Self&) = default;

    NoMove(Self&&) noexcept = delete;
    Self& operator=(Self&&) noexcept = delete;
};

/**
 * @class NoCopyMove
 * @brief 禁止拷贝和移动
 */
class NoCopyMove {
protected:
    using Self = NoCopyMove;

    constexpr NoCopyMove() = default;
    ~NoCopyMove() = default;

    NoCopyMove(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    NoCopyMove(Self&&) noexcept = delete;
    Self& operator=(Self&&) noexcept = delete;
};

/**
 * @brief 单例类
 */
template <typename T>
class Singleton {
    Singleton() = delete;
    ~Singleton() = delete;

public:
    [[nodiscard]] static auto instance() -> T& {
        static T instance;
        return instance;
    }
};

} // namespace my

#endif // RICKY_CPP_NOCOPY_HPP