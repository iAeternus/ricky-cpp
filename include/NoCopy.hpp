/**
 * @file NoCopy.hpp
 * @brief RAII资源管理基类
 * @author Ricky
 * @date 2024/12/19
 * @version 1.1
 * 
 * 本文件提供了：
 * - 禁止拷贝的基类
 * - 禁止移动的基类
 * - 禁止拷贝和移动的基类
 * - 仅允许移动的基类
 */
#ifndef NO_COPY_HPP
#define NO_COPY_HPP

namespace my {

/**
 * @brief 禁止拷贝的基类
 * 
 * 继承此类的子类将无法进行拷贝操作，但可以进行移动操作。
 * 适用于独占资源的类型。
 */
class NoCopyable {
protected:
    using Self = NoCopyable;

    constexpr NoCopyable() = default;
    ~NoCopyable() = default;
    
    NoCopyable(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    
    NoCopyable(Self&&) noexcept = default;
    Self& operator=(Self&&) noexcept = default;
};

/**
 * @brief 禁止移动的基类
 * 
 * 继承此类的子类将无法进行移动操作，但可以进行拷贝操作。
 * 适用于共享资源的类型。
 */
class NoMoveable {
protected:
    using Self = NoMoveable;

    constexpr NoMoveable() = default;
    ~NoMoveable() = default;
    
    NoMoveable(const Self&) = default;
    Self& operator=(const Self&) = default;
    
    NoMoveable(Self&&) noexcept = delete;
    Self& operator=(Self&&) noexcept = delete;
};

/**
 * @brief 禁止拷贝和移动的基类
 * 
 * 继承此类的子类将无法进行拷贝和移动操作。
 * 适用于单例类型或全局资源管理器。
 */
class NonCopyMoveable {
protected:
    using Self = NonCopyMoveable;

    constexpr NonCopyMoveable() = default;
    ~NonCopyMoveable() = default;
    
    NonCopyMoveable(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    
    NonCopyMoveable(Self&&) noexcept = delete;
    Self& operator=(Self&&) noexcept = delete;
};

/**
 * @brief 仅允许移动的基类
 * 
 * 继承此类的子类只能进行移动操作，不能进行拷贝操作。
 * 适用于独占资源的类型，如智能指针、文件句柄等。
 */
class MoveOnly {
protected:
    using Self = MoveOnly;

    constexpr MoveOnly() = default;
    virtual ~MoveOnly() = default;
    
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

// 兼容性别名
using NoCopy = NonCopyMoveable;

} // namespace my

#endif // RICKY_CPP_NOCOPY_HPP