/**
 * @brief 断言工具
 * @author Ricky
 * @date 2024/12/13
 * @version 1.0
 */
#ifndef ASSERTIONS_HPP
#define ASSERTIONS_HPP

#include "math_utils.hpp"
#include "Exception.hpp"
#include <exception>

namespace my::test {

/**
 * @brief 断言异常，断言失败时抛出
 */
class AssertionFailedException : public std::runtime_error {
public:
    explicit AssertionFailedException(const CString& message) :
            std::runtime_error(message.data()) {}

    explicit AssertionFailedException(CString&& message) :
            std::runtime_error(std::move(message).data()) {}
};

class Assertions : public Object<Assertions> {
public:
    /**
     * @brief 断言表达式是否为true，否则抛出异常
     */
    static void assertTrue(bool expression, CString&& message = "") {
        if (!expression) {
            fail("Expected true, but got false.", std::forward<CString>(message));
        }
    }

    /**
     * @brief 断言表达式是否为false，否则抛出异常
     */
    static void assertFalse(bool expression, CString&& message = "") {
        if (expression) {
            fail("Expected false, but got true.", std::forward<CString>(message));
        }
    }

    /**
     * @brief 断言两个值相等，否则抛出异常
     */
    template <Assertable T, Assertable U>
    static void assertEquals(const T& expected, const U& actual, CString&& message = "") {
        if (expected.__cmp__(actual) != 0) {
            fail(std::format("Expected {}, but got {}", expected.__str__(), actual.__str__(), std::forward<CString>(message)));
        }
    }

    template <StdPrintable T, StdPrintable U>
    static void assertEquals(const T& expected, const U& actual, CString&& message = "") {
        if constexpr (is_same<T, f32, f64, f128>) {
            if (math::fcmp(expected, actual) != 0) {
                fail(std::format("Expected {}, but got {}", expected, actual, std::forward<CString>(message)));
            }
        } else {
            if (expected != actual) {
                fail(std::format("Expected {}, but got {}", expected, actual, std::forward<CString>(message)));
            }
        }
    }

    /**
     * @brief 断言两个值不相等，否则抛出异常
     */
    template <Assertable T, Assertable U>
    static void assertNotEquals(const T& unexpected, const U& actual, CString&& message = "") {
        if (unexpected.__cmp__(actual) == 0) {
            fail(std::format("Expected not {}, but got {}", unexpected.__str__(), actual.__str__()), std::forward<CString>(message));
        }
    }

    template <StdPrintable T, StdPrintable U>
    static void assertNotEquals(const T& unexpected, const U& actual, CString&& message = "") {
        if constexpr (is_same<T, f32, f64, f128>) {
            if (math::fcmp(unexpected, actual) == 0) {
                fail(std::format("Expected not {}, but got {}", unexpected, actual), std::forward<CString>(message));
            }
        } else {
            if (unexpected == actual) {
                fail(std::format("Expected not {}, but got {}", unexpected, actual), std::forward<CString>(message));
            }
        }
    }

    static void assertThrows(CString&& expectedMessage, Runnable&& func) {
        try {
            func();
            fail(std::format("Expected exception with message \"{}\" but no exception was thrown", expectedMessage));
        } catch (const Exception& ex) {
            if (expectedMessage != ex.message()) {
                fail(std::format("Expected exception message \"{}\" but got \"{}\"", expectedMessage, ex.message()));
            }
        } catch (...) {
            fail(std::format("Expected exception with message \"{}\" but got an unknown exception", expectedMessage));
        }
    }

    static void assertThrows(const char* expectedMessage, Runnable&& func) {
        try {
            func();
            fail(std::format("Expected exception with message \"{}\" but no exception was thrown", expectedMessage));
        } catch (const Exception& ex) {
            if (std::strcmp(expectedMessage, ex.message())) {
                fail(std::format("Expected exception message \"{}\" but got \"{}\"", expectedMessage, ex.message()));
            }
        } catch (...) {
            fail(std::format("Expected exception with message \"{}\" but got an unknown exception", expectedMessage));
        }
    }

private:
    static void fail(const CString& failureMessage, const CString& additionalMessage = "") {
        std::stringstream stream;
        stream << "Assertion Failed: " << failureMessage;
        if (!additionalMessage.empty()) {
            stream << " " << additionalMessage;
        }
        throw AssertionFailedException(stream.str());
    }
};

} // namespace my::test

#endif // ASSERTIONS_HPP