/**
 * @brief 断言工具
 * @author Ricky
 * @date 2024/12/13
 * @version 1.0
 */
#ifndef ASSERTIONS_HPP
#define ASSERTIONS_HPP

#include "ricky_concepts.hpp"
#include "CString.hpp"
#include "Object.hpp"

#include <stdexcept>

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
    template <Assertable T>
    static void assertEquals(const T& expected, const T& actual, CString&& message = "") {
        if (expected.__cmp__(actual) != 0) {
            fail(std::format("Expected {}, but got {}", expected.__str__(), actual.__str__(), std::forward<CString>(message)));
        }
    }

    template <StdPrintable T>
    static void assertEquals(const T& expected, const T& actual, CString&& message = "") {
        if (expected != actual) {
            fail(std::format("Expected {}, but got {}", expected, actual, std::forward<CString>(message)));
        }
    }

    /**
     * @brief 断言两个值不相等，否则抛出异常
     */
    template <Assertable T>
    static void assertNotEquals(const T& unexpected, const T& actual, CString&& message = "") {
        if (unexpected.__cmp__(actual) == 0) {
            fail(std::format("Expected not {}, but got {}", unexpected.__str__(), actual.__str__()), std::forward<CString>(message));
        }
    }

    template <StdPrintable T>
    static void assertNotEquals(const T& unexpected, const T& actual, CString&& message = "") {
        if (unexpected == actual) {
            fail(std::format("Expected not {}, but got {}", unexpected, actual), std::forward<CString>(message));
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