/**
 * @brief 格式化相关支持
 * @author Ricky
 * @date 2025/10/15
 * @version 1.0
 */

#ifndef RICKY_FORMAT_HPP
#define RICKY_FORMAT_HPP

#include <format>
#include <source_location>

namespace my {

/**
 * @brief 格式化字符串包装器
 */
template <typename... Args>
struct basic_format_string_wrapper {
    std::format_string<Args...> fmt;
    std::source_location loc;

    template <typename T>
        requires std::convertible_to<T, std::string_view>
    consteval basic_format_string_wrapper(const T& s, std::source_location loc = std::source_location::current()) :
            fmt(s), loc(loc) {
    }
};

/**
 * @brief 重命名格式化字符串包装器，使用std::type_identity_t避免自动类型推导
 */
template <typename... Args>
using format_string_wrapper = basic_format_string_wrapper<std::type_identity_t<Args>...>;

} // namespace my

/**
 * @brief 为std::source_location提供格式化支持
 */
template <>
struct std::formatter<std::source_location> : std::formatter<std::string_view> {
    auto format(const std::source_location& loc, format_context& ctx) const {
        return std::formatter<std::string_view>::format(std::format("{}:{}", loc.file_name(), loc.line()), ctx);
    }
};

#endif // RICKY_CPP_RICKY_FORMAT_HPP
