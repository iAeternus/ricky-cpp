/**
 * @brief 提示错误信息，TODO 已弃用！
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef RAISE_ERROR_HPP
#define RAISE_ERROR_HPP

#include "Printer.hpp"

#include <source_location>

namespace my {

template <Printable T>
inline void warn_assert(bool condition, const T& msg, const STD source_location& loc = STD source_location::current()) {
    if (!condition) {
        io::my_warner(
            STD format("file: {}\nline, column: {}, {}\nfunction_name: {}\nerror:",
                       loc.file_name(),
                       loc.line(),
                       loc.column(),
                       loc.function_name()),
            msg);
    }
}

template <Printable T>
inline void error_exec(const T& msg, const STD source_location& loc = STD source_location::current()) {
    io::my_error(
        STD format("file: {}\nline, column: {}, {}\nfunction_name: {}\n",
                   loc.file_name(),
                   loc.line(),
                   loc.column(),
                   loc.function_name()),
        msg);
    throw STD runtime_error("");
}

#define Error(errorname, msg) error_exec(STD format("{}: {}", errorname, msg))

// #define KeyError(msg) Error("KeyError", msg)

// #define ValueError(msg) Error("ValueError", msg)

// #define TypeError(msg) Error("TypeError", msg)

// #define RuntimeError(msg) Error("RuntimeError", msg)

// #define NotImplementedError(msg) Error("NotImplementedError", msg)

// #define FileNotFoundError(msg) Error("FileNotFoundError", msg)

// #define PermissionError(msg) Error("PermissionError", msg)

// #define EncodingError(msg) Error("EncodingError", msg)

// #define SystemError(msg) Error("SystemError", msg)

// #define TestError(msg) Error("TestError", msg)

} // namespace my

#endif // RAISE_ERROR_HPP