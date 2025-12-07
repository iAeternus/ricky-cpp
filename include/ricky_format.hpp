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
 * @brief 重命名格式化字符串包装器，使用std::type_identity_t避免自动类型推导 TODO 问题似乎出在 std::type_identity_t 上
 */
template <typename... Args>
using format_string_wrapper = basic_format_string_wrapper<std::type_identity_t<Args>...>;

} // namespace my

/**
 * @brief 为std::source_location提供格式化支持 TODO 这里需要重新思考
 */
template <typename CharT>
struct std::formatter<std::source_location, CharT> : std::formatter<std::basic_string_view<CharT>, CharT> {
    using format_context = std::basic_format_context<typename std::basic_format_context<std::back_insert_iterator<std::basic_string<CharT>>, CharT>::iterator, CharT>;

    auto format(const std::source_location& loc, format_context& ctx) const {
        auto filename = loc.file_name();
        auto line = loc.line();

        std::basic_string<CharT> str;
        if constexpr (std::is_same_v<CharT, char>) {
            str = std::format("{}:{}", filename, line);
        }

        return std::formatter<std::basic_string_view<CharT>, CharT>::format(str, ctx);
    }
};

#endif // RICKY_CPP_RICKY_FORMAT_HPP
