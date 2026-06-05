/**
 * @brief 断言工具
 * @author Ricky
 * @date 2024/12/13
 * @version 1.0
 */
#ifndef ASSERTIONS_HPP
#define ASSERTIONS_HPP

#include "float_compare.hpp"
#include "my_exception.hpp"
#include "cstring.hpp"
#include "my_format.hpp"

namespace my::test {

class Assertions {
public:
    static auto fail(std::string_view message, std::source_location loc = SRC_LOC) -> void {
        throw assertion_failed_exception("{}\n  at {}:{}:{}\n  function: {}",
                                         message,
                                         loc.file_name(),
                                         loc.line(),
                                         loc.column(),
                                         loc.function_name());
    }

    template <typename... Args>
    static auto fail_fmt(format_string_wrapper<Args...> fmt_w, Args&&... args) -> void {
        fail(std::format(fmt_w.fmt, std::forward<Args>(args)...), fmt_w.loc);
    }

    static void assert_true(bool value, std::source_location loc = SRC_LOC) {
        if (!value) {
            fail("Assertion failed: expected true but got false", loc);
        }
    }

    static void assert_false(bool value, std::source_location loc = SRC_LOC) {
        if (value) {
            fail("Assertion failed: expected false but got true", loc);
        }
    }

    template <typename T>
        requires std::is_pointer_v<T>
    static void assert_null(T ptr, std::source_location loc = SRC_LOC) {
        if (ptr != nullptr) {
            fail("Assertion failed: expected nullptr but got non-null pointer", loc);
        }
    }

    template <typename T>
        requires std::is_pointer_v<T>
    static void assert_not_null(T ptr, std::source_location loc = SRC_LOC) {
        if (ptr == nullptr) {
            fail("Assertion failed: expected non-null pointer but got nullptr", loc);
        }
    }

    template <typename T, typename U>
        requires requires(T a, U b) { a == b; } || requires(T a, U b) { a.cmp(b); }
    static void assert_equals(const T& expected, const U& actual, std::source_location loc = SRC_LOC) {
        auto is_equal = [&]() -> bool {
            if constexpr (requires { fcmp(expected, actual); }) {
                return fcmp(expected, actual) == 0;
            } else if constexpr (requires { expected == actual; }) {
                return expected == actual;
            } else {
                return expected.cmp(actual) == 0;
            }
        }();
        if (!is_equal) {
            fail(std::format("Assertion failed: expected {} but got {}", to_debug_string(expected), to_debug_string(actual)), loc);
        }
    }

    template <typename T, typename U>
        requires requires(T a, U b) { a == b; } || requires(T a, U b) { a.cmp(b); }
    static void assert_not_equals(const T& unexpected, const U& actual, std::source_location loc = SRC_LOC) {
        auto is_equal = [&]() -> bool {
            if constexpr (requires { fcmp(unexpected, actual); }) {
                return fcmp(unexpected, actual) == 0;
            } else if constexpr (requires { unexpected == actual; }) {
                return unexpected == actual;
            } else {
                return unexpected.cmp(actual) == 0;
            }
        }();
        if (is_equal) {
            fail(std::format("Assertion failed: expected value != {}", to_debug_string(unexpected)), loc);
        }
    }

    template <typename Ex = std::exception, std::invocable F>
    static void assert_throws(F&& func, std::source_location loc = SRC_LOC) {
        try {
            func();
        } catch (const Ex&) {
            return;
        } catch (...) {
            fail(std::format("Assertion failed: expected exception {} but got other exception", typeid(Ex).name()), loc);
        }
        fail(std::format("Assertion failed: expected exception {} but no exception thrown", typeid(Ex).name()), loc);
    }

    template <typename Ex = std::exception, std::invocable F>
    static void assert_throws(CStringView expected_msg, F&& func, std::source_location loc = SRC_LOC) {
        auto get_msg = [](const std::exception& e) -> std::string_view {
            if (auto* my_ex = dynamic_cast<const my::Exception*>(&e)) {
                return my_ex->message().data();
            }
            return e.what();
        };
        try {
            func();
        } catch (const Ex& ex) {
            auto sv_expected = std::string_view(expected_msg.begin(), expected_msg.length());
            if (sv_expected != get_msg(ex)) {
                fail(std::format("Assertion failed: expected exception message \"{}\" but got \"{}\"", sv_expected, get_msg(ex)), loc);
            }
            return;
        } catch (...) {
            fail(std::format("Assertion failed: expected {}(\"{}\") but got other exception", typeid(Ex).name(), expected_msg.begin()), loc);
        }
        fail(std::format("Assertion failed: expected {}(\"{}\") but no exception thrown", typeid(Ex).name(), expected_msg.begin()), loc);
    }

    template <typename Ex = std::exception, std::invocable F>
    static void assert_not_throws(F&& func, std::source_location loc = SRC_LOC) {
        auto get_msg = [](const std::exception& e) -> std::string_view {
            if (auto* my_ex = dynamic_cast<const my::Exception*>(&e)) {
                return my_ex->message().data();
            }
            return e.what();
        };
        try {
            func();
        } catch (const Ex& ex) {
            fail(std::format("Assertion failed: expected NO exception {}, but caught: {}", typeid(Ex).name(), get_msg(ex)), loc);
        }
    }

    template <typename Ex = std::exception, std::invocable F>
    static void assert_not_throws(CStringView msg, F&& func, std::source_location loc = SRC_LOC) {
        auto get_msg = [](const std::exception& e) -> std::string_view {
            if (auto* my_ex = dynamic_cast<const my::Exception*>(&e)) {
                return my_ex->message().data();
            }
            return e.what();
        };
        try {
            func();
        } catch (const Ex& ex) {
            fail(std::format("Assertion failed: expected NO exception {} ({}), but got: {}", typeid(Ex).name(), msg.begin(), get_msg(ex)), loc);
        }
    }

private:
    template <typename T>
    static std::string to_debug_string(const T& value) {
        if constexpr (Formattable<T>) {
            return std::format("{}", value);
        } else if constexpr (requires { std::format("{}", value.to_string()); }) {
            return std::format("{}", value.to_string());
        } else {
            return std::format("<{}>", typeid(T).name());
        }
    }
};

} // namespace my::test

#endif // ASSERTIONS_HPP
