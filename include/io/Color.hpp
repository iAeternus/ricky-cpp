/**
 * @brief 颜色常量定义
 * @author Ricky
 * @date 2025/7/12
 * @version 1.0
 */
#ifndef COLOR_HPP
#define COLOR_HPP

namespace my::io {

/**
 * @brief 控制台打印颜色转义码
 */
class Color {
public:
    constexpr static const char* CLOSE = "\033[0m";
    constexpr static const char* BLACK = "\033[30m";
    constexpr static const char* RED = "\033[31m";
    constexpr static const char* GREEN = "\033[32m";
    constexpr static const char* YELLOW = "\033[33m";
    constexpr static const char* BLUE = "\033[34m";
    constexpr static const char* PURPLE = "\033[35m";
    constexpr static const char* DEEPGREEN = "\033[36m";
    constexpr static const char* WHITE = "\033[37m";
    constexpr static const char* AQUA = "\033[94m";
};

} // namespace my::io

#endif // COLOR_HPP