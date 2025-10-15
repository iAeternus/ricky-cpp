/**
 * @brief 颜色常量定义
 * @author Ricky
 * @date 2025/7/12
 * @version 1.0
 */
#ifndef COLOR_HPP
#define COLOR_HPP

namespace my::color {

/**
 * @brief 控制台打印颜色转义码
 */
class Color {
public:
    constexpr static auto CLOSE = "\033[0m";
    constexpr static auto BLACK = "\033[30m";
    constexpr static auto RED = "\033[31m";
    constexpr static auto GREEN = "\033[32m";
    constexpr static auto YELLOW = "\033[33m";
    constexpr static auto BLUE = "\033[34m";
    constexpr static auto PURPLE = "\033[35m";
    constexpr static auto DEEPGREEN = "\033[36m";
    constexpr static auto WHITE = "\033[37m";
    constexpr static auto CYAN = "\033[46m";
    constexpr static auto AQUA = "\033[94m";
};

} // namespace my::color

#endif // COLOR_HPP