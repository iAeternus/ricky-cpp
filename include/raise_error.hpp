/**
 * @brief 提示错误信息
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0 
 */
#ifndef RAISE_ERROR_HPP
#define RAISE_ERROR_HPP

#include "printer.hpp"

#include <source_location>

namespace my {

template <Printable T>
def warn_assert(bool condition, const T& msg, const std::source_location& loc = std::source_location::current()) {
    if (!condition) {
        my_warner(std::format("file: {}\nline, column: {}, {}\nfunction_name: {}\nerror:",
                              loc.file_name(),
                              loc.line(),
                              loc.column(),
                              loc.function_name()),
                  msg);
    }
}

template <Printable T>
def error_exec(const T& msg, const std::source_location& loc = std::source_location::current()) {
    my_error(
        std::format("file: {}\nline, column: {}, {}\nfunction_name: {}\n",
                    loc.file_name(),
                    loc.line(),
                    loc.column(),
                    loc.function_name()),
        msg);
    throw std::runtime_error("");
}

#define Error(errorname, msg) error_exec(std::format("{}: {}", errorname, msg))

#define KeyError(msg) Error("KeyError", msg)

#define ValueError(msg) Error("ValueError", msg)

#define TypeError(msg) Error("TypeError", msg)

#define RuntimeError(msg) Error("RuntimeError", msg)

#define NotImplementedError(msg) Error("NotImplementedError", msg)

#define FileNotFoundError(msg) Error("FileNotFoundError", msg)

#define PermissionError(msg) Error("PermissionError", msg)

#define EncodingError(msg) Error("EncodingError", msg)

#define SystemError(msg) Error("SystemError", msg)

} // namespace my

#endif // RAISE_ERROR_HPP