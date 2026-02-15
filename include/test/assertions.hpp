/**
 * @brief 断言工具
 * @author Ricky
 * @date 2024/12/13
 * @version 1.0
 */
#ifndef ASSERTIONS_HPP
#define ASSERTIONS_HPP

#include "math_utils.hpp"

namespace my::test {

class Assertions {
public:
    static auto fail2(std::string_view message, std::source_location loc = SRC_LOC) -> void {
        throw assertion_failed_exception("{}\n  at {}:{}:{}\n  function: {}",
                                         message,
                                         loc.file_name(),
                                         loc.line(),
                                         loc.column(),
                                         loc.function_name());
    }

    template <typename... Args>
    static auto fail_fmt(format_string_wrapper<Args...> fmt_w, Args&&... args) -> void {
        fail2(std::format(fmt_w.fmt, std::forward<Args>(args)...), fmt_w.loc);
    }

    static void assert_true(bool value, std::source_location loc = std::source_location::current()) {
        if (!value) {
            fail2("Assertion failed: expected true but got false", loc);
        }
    }

    static void assert_false(bool value, std::source_location loc = std::source_location::current()) {
        if (value) {
            fail2("Assertion failed: expected false but got true", loc);
        }
    }

    template <typename T>
        requires std::is_pointer_v<T>
    static void assert_null(T ptr, std::source_location loc = std::source_location::current()) {
        if (ptr != nullptr) {
            fail2("Assertion failed: expected nullptr but got non-null pointer", loc);
        }
    }

    template <typename T>
        requires std::is_pointer_v<T>
    static void assert_not_null(T ptr, std::source_location loc = std::source_location::current()) {
        if (ptr == nullptr) {
            fail2("Assertion failed: expected non-null pointer but got nullptr", loc);
        }
    }

    template <typename T, typename U>
        requires requires(T a, U b) { a == b; }
    static void assert_equals(const T& expected, const U& actual, std::source_location loc = std::source_location::current()) {
        if (!(expected == actual)) {
            fail2(std::format("Assertion failed: expected {} but got {}", expected, actual), loc);
        }
    }

    template <typename T, typename U>
        requires requires(T a, U b) { a != b; }
    static void assert_not_equals(const T& unexpected, const U& actual, std::source_location loc = std::source_location::current()) {
        if (unexpected == actual) {
            fail2(std::format("Assertion failed: expected value != {}", unexpected), loc);
        }
    }

    template <typename Ex, typename F>
        requires std::invocable<F>
    static void assert_throws(F&& func, std::source_location loc = std::source_location::current()) {
        try {
            func();
        } catch (const Ex&) {
            return;
        } catch (...) {
            fail2(std::format("Assertion failed: expected exception {} but got other exception", typeid(Ex).name()), loc);
        }
        fail2(std::format("Assertion failed: expected exception {} but no exception thrown", typeid(Ex).name()), loc);
    }

    template <typename Ex, typename F>
        requires std::invocable<F> // TODO CStringView需要重新思考
    static void assert_throws(CStringView expected_msg, F&& func, std::source_location loc = std::source_location::current()) {
        try {
            func();
        } /*catch (const Ex& ex) {
            if (expected_msg != ex.what()) {
                fail2(std::format("Assertion failed: expected exception message \"{}\" but got \"{}\"", expected_msg, ex.what()), loc);
            }
            return;
        }*/
        catch (const Exception& ex) {
            if (expected_msg != ex.message().data()) {
                fail2(std::format("Assertion failed: expected exception message \"{}\" but got \"{}\"", expected_msg, ex.what()), loc);
            }
            return;
        } catch (...) {
            fail2(std::format("Assertion failed: expected {}(\"{}\") but got other exception", typeid(Ex).name(), expected_msg), loc);
        }

        fail2(std::format("Assertion failed: expected {}(\"{}\") but no exception thrown", typeid(Ex).name(), expected_msg), loc);
    }

    template <typename Ex, typename F>
        requires std::invocable<F>
    static void assert_not_throws(F&& func, std::source_location loc = std::source_location::current()) {
        try {
            func();
        } catch (const Ex& ex) {
            fail2(std::format("Assertion failed: expected NO exception {}, but caught: {}", typeid(Ex).name(), ex.what()), loc);
        }
    }

    template <typename Ex, typename F>
        requires std::invocable<F>
    static void assert_not_throws(CStringView msg, F&& func, std::source_location loc = std::source_location::current()) {
        try {
            func();
        } catch (const Ex& ex) {
            fail2(std::format("Assertion failed: expected NO exception {} ({}), but got: {}", typeid(Ex).name(), msg, ex.what()), loc);
        }
    }

public:
    /**
     * @brief 断言表达式是否为true，否则抛出异常，不加用户消息
     * @param expression 要断言的表达式
     * @param loc 断言发生的位置
     */
    static void assertTrue(bool expression, std::source_location loc = SRC_LOC) {
        if (!expression) {
            fail("Expected true, but got false.", loc);
        }
    }

    /**
     * @brief 断言表达式是否为true，否则抛出异常
     * @param expression 要断言的表达式
     * @param message 断言失败时的错误信息
     * @param loc 断言发生的位置
     */
    template <typename... Args>
    static void assertTrue(bool expression, format_string_wrapper<Args...> fmt_w, Args... args) {
        if (!expression) {
            fail_with_prefix("Expected true, but got false. ", fmt_w, std::forward<Args>(args)...);
        }
    }

    /**
     * @brief 断言表达式是否为false，否则抛出异常，不加用户消息
     * @param expression 要断言的表达式
     * @param loc 断言发生的位置
     */
    static void assertFalse(bool expression, std::source_location loc = SRC_LOC) {
        if (expression) {
            fail("Expected false, but got true.", loc);
        }
    }

    /**
     * @brief 断言表达式是否为false，否则抛出异常
     * @param expression 要断言的表达式
     * @param message 断言失败时的错误信息
     * @param loc 断言发生的位置
     */
    template <typename... Args>
    static void assertFalse(bool expression, format_string_wrapper<Args...> fmt_w, Args... args) {
        if (expression) {
            fail_with_prefix("Expected false, but got true. ", fmt_w, std::forward<Args>(args)...);
        }
    }

    template <typename T>
    static void assertNull(T* ptr, std::source_location loc = SRC_LOC) {
        if (ptr != nullptr) {
            fail("Expected nullptr, but got !nullptr.", loc);
        }
    }

    template <typename T>
    static void assertNotNull(T* ptr, std::source_location loc = SRC_LOC) {
        if (ptr == nullptr) {
            fail("Expected !nullptr, but got nullptr.", loc);
        }
    }

    /**
     * @brief 断言两个值相等，否则抛出异常，不加用户消息
     * @note 对于自定义类型，要求实现__cmp__和__str__方法
     * @param expected 期望的值
     * @param actual 实际的值
     * @param loc 断言发生的位置
     */
    template <Assertable T, Assertable U>
    static void assertEquals(const T& expected, const U& actual, std::source_location loc = SRC_LOC) {
        if (expected.__cmp__(actual) != 0) {
            fail(std::format("Expected {}, but got {}.", expected.__str__(), actual.__str__()), loc);
        }
    }

    /**
     * @brief 断言两个值相等，否则抛出异常
     * @note 对于自定义类型，要求实现__cmp__和__str__方法
     * @param expected 期望的值
     * @param actual 实际的值
     * @param message 断言失败时的错误信息
     * @param loc 断言发生的位置
     */
    template <Assertable T, Assertable U, typename... Args>
    static void assertEquals(const T& expected, const U& actual, format_string_wrapper<Args...> fmt_w, Args... args) {
        if (expected.__cmp__(actual) != 0) {
            auto prefix = std::format("Expected {}, but got {}. ", expected.__str__(), actual.__str__());
            fail_with_prefix(prefix, fmt_w, std::forward<Args>(args)...);
        }
    }

    /**
     * @brief 断言两个值相等，否则抛出异常，不加用户消息
     * @note 对于浮点数，使用math::fcmp进行比较
     * @param expected 期望的值
     * @param actual 实际的值
     * @param loc 断言发生的位置
     */
    template <StdPrintable T, StdPrintable U>
    static void assertEquals(const T& expected, const U& actual, std::source_location loc = SRC_LOC) {
        bool is_equal = false;
        if constexpr (is_same<T, f32, f64, f128>) {
            is_equal = math::fcmp(expected, actual) == 0;
        } else {
            is_equal = expected == actual;
        }

        if (!is_equal) {
            fail(std::format("Expected {}, but got {}.", expected, actual), loc);
        }
    }

    /**
     * @brief 断言两个值相等，否则抛出异常
     * @note 对于浮点数，使用math::fcmp进行比较
     * @param expected 期望的值
     * @param actual 实际的值
     * @param message 断言失败时的错误信息
     * @param loc 断言发生的位置
     */
    template <StdPrintable T, StdPrintable U, typename... Args>
    static void assertEquals(const T& expected, const U& actual, format_string_wrapper<Args...> fmt_w, Args... args) {
        bool is_equal = false;
        if constexpr (is_same<T, f32, f64, f128>) {
            is_equal = math::fcmp(expected, actual) == 0;
        } else {
            is_equal = expected == actual;
        }

        if (!is_equal) {
            auto user_msg = std::format(fmt_w.fmt, std::forward<Args>(args)...);
            auto full_msg = std::format("Expected {}, but got {}. {}", expected, actual, user_msg);
            fail(std::move(full_msg), fmt_w.loc);
        }
    }

    /**
     * @brief 断言两个值不相等，否则抛出异常，不加用户消息
     * @note 对于自定义类型，要求实现__cmp__和__str__方法
     * @param unexpected 不期望的值
     * @param actual 实际的值
     * @param loc 断言发生的位置
     */
    template <Assertable T, Assertable U>
    static void assertNotEquals(const T& unexpected, const U& actual, std::source_location loc = SRC_LOC) {
        if (unexpected.__cmp__(actual) == 0) {
            fail(std::format("Expected not {}, but got {}.", unexpected.__str__(), actual.__str__()), loc);
        }
    }

    /**
     * @brief 断言两个值不相等，否则抛出异常
     * @note 对于自定义类型，要求实现__cmp__和__str__方法
     * @param unexpected 不期望的值
     * @param actual 实际的值
     * @param message 断言失败时的错误信息
     * @param loc 断言发生的位置
     */
    template <Assertable T, Assertable U, typename... Args>
    static void assertNotEquals(const T& unexpected, const U& actual, format_string_wrapper<Args...> fmt_w, Args... args) {
        if (unexpected.__cmp__(actual) == 0) {
            auto prefix = std::format("Expected not {}, but got {}. ", unexpected.__str__(), actual.__str__());
            fail_with_prefix(prefix, fmt_w, std::forward<Args>(args)...);
        }
    }

    /**
     * @brief 断言两个值不相等，否则抛出异常，不加用户消息
     * @note 对于浮点数，使用math::fcmp进行比较
     * @param unexpected 不期望的值
     * @param actual 实际的值
     * @param message 断言失败时的错误信息
     * @param loc 断言发生的位置
     */
    template <StdPrintable T, StdPrintable U>
    static void assertNotEquals(const T& unexpected, const U& actual, std::source_location loc = SRC_LOC) {
        bool is_equal = false;
        if constexpr (is_same<T, f32, f64, f128>) {
            is_equal = math::fcmp(unexpected, actual) == 0;
        } else {
            is_equal = unexpected == actual;
        }

        if (is_equal) {
            fail(std::format("Expected not {}, but got {}.", unexpected, actual), loc);
        }
    }

    /**
     * @brief 断言两个值不相等，否则抛出异常
     * @note 对于浮点数，使用math::fcmp进行比较
     * @param unexpected 不期望的值
     * @param actual 实际的值
     * @param message 断言失败时的错误信息
     * @param loc 断言发生的位置
     */
    template <StdPrintable T, StdPrintable U, typename... Args>
    static void assertNotEquals(const T& unexpected, const U& actual, format_string_wrapper<Args...> fmt_w, Args... args) {
        bool is_equal = false;
        if constexpr (is_same<T, f32, f64, f128>) {
            is_equal = math::fcmp(unexpected, actual) == 0;
        } else {
            is_equal = unexpected == actual;
        }

        if (is_equal) {
            auto user_msg = std::format(fmt_w.fmt, std::forward<Args>(args)...);
            auto full_msg = std::format("Expected not {}, but got {}. {}", unexpected, actual, user_msg);
            fail(std::move(full_msg), fmt_w.loc);
        }
    }

    /**
     * @brief 断言抛出异常，否则抛出异常
     * @param expectedMessage 期望的异常消息
     * @param func 要执行的函数
     * @param loc 断言发生的位置
     */
    static void assertThrows(CString&& expectedMessage, Runnable&& func, std::source_location loc = SRC_LOC) {
        bool no_exception_throw = false;
        try {
            func();
            no_exception_throw = true;
        } catch (const Exception& ex) {
            if (expectedMessage != ex.message()) {
                fail(std::format("Expected exception message \"{}\" but got \"{}\"", expectedMessage, ex.message()), loc);
            }
        } catch (...) {
            fail(std::format("Expected exception with message \"{}\" but got an unknown exception", expectedMessage), loc);
        }

        if (no_exception_throw) {
            fail(std::format("Expected exception with message \"{}\" but no exception was thrown", expectedMessage), loc);
        }
    }

    /**
     * @brief 断言抛出异常，否则抛出异常
     * @param expectedMessage 期望的异常消息
     * @param func 要执行的函数
     * @param loc 断言发生的位置
     */
    static void assertThrows(const char* expectedMessage, Runnable&& func, std::source_location loc = SRC_LOC) {
        bool no_exception_throw = false;
        try {
            func();
            no_exception_throw = true;
        } catch (const Exception& ex) {
            if (std::strcmp(expectedMessage, ex.message())) {
                fail(std::format("Expected exception message \"{}\" but got \"{}\"", expectedMessage, ex.message()), loc);
            }
        } catch (...) {
            fail(std::format("Expected exception with message \"{}\" but got an unknown exception", expectedMessage), loc);
        }

        if (no_exception_throw) {
            fail(std::format("Expected exception with message \"{}\" but no exception was thrown", expectedMessage), loc);
        }
    }

    /**
     * @brief 统一的失败处理函数
     */
    static void fail(CString&& message, std::source_location loc) {
        throw Exception(ExceptionType::AssertionFailedException, std::move(message), loc);
    }

private:
    /**
     * @brief 带前缀的失败处理
     */
    template <typename... Args>
    static void fail_with_prefix(const char* prefix,
                                 format_string_wrapper<Args...> user_fmt_w,
                                 Args&&... user_args) {
        auto user_msg = std::format(user_fmt_w.fmt, std::forward<Args>(user_args)...);
        auto full_msg = CString(prefix) + user_msg;
        fail(std::move(full_msg), user_fmt_w.loc);
    }
};

} // namespace my::test

#endif // ASSERTIONS_HPP