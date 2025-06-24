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

namespace my::test {

class Assertions : public Object<Assertions> {
public:
    /**
     * @brief 断言表达式是否为true，否则抛出异常
     */
    static void assertTrue(bool expression, CString&& message = "", std::source_location loc = SRC_LOC) {
        if (!expression) {
            // fail("Expected true, but got false.", std::forward<CString>(message), loc);
            fail(std::format("Expected true, but got false. {}", std::forward<CString>(message)), loc);
        }
    }

    /**
     * @brief 断言表达式是否为false，否则抛出异常
     */
    static void assertFalse(bool expression, CString&& message = "", std::source_location loc = SRC_LOC) {
        if (expression) {
            fail(std::format("Expected false, but got true. {}", std::forward<CString>(message)), loc);
        }
    }

    /**
     * @brief 断言两个值相等，否则抛出异常
     */
    template <Assertable T, Assertable U>
    static void assertEquals(const T& expected, const U& actual, CString&& message = "", std::source_location loc = SRC_LOC) {
        if (expected.__cmp__(actual) != 0) {
            fail(std::format("Expected {}, but got {}. {}", expected.__str__(), actual.__str__(), std::forward<CString>(message)), loc);
        }
    }

    template <StdPrintable T, StdPrintable U>
    static void assertEquals(const T& expected, const U& actual, CString&& message = "", std::source_location loc = SRC_LOC) {
        if constexpr (is_same<T, f32, f64, f128>) {
            if (math::fcmp(expected, actual) != 0) {
                fail(std::format("Expected {}, but got {}. {}", expected, actual, std::forward<CString>(message)), loc);
            }
        } else {
            if (expected != actual) {
                fail(std::format("Expected {}, but got {}. {}", expected, actual, std::forward<CString>(message)), loc);
            }
        }
    }

    /**
     * @brief 断言两个值不相等，否则抛出异常
     */
    template <Assertable T, Assertable U>
    static void assertNotEquals(const T& unexpected, const U& actual, CString&& message = "", std::source_location loc = SRC_LOC) {
        if (unexpected.__cmp__(actual) == 0) {
            fail(std::format("Expected not {}, but got {}. {}", unexpected.__str__(), actual.__str__(), std::forward<CString>(message)), loc);
        }
    }

    template <StdPrintable T, StdPrintable U>
    static void assertNotEquals(const T& unexpected, const U& actual, CString&& message = "", std::source_location loc = SRC_LOC) {
        if constexpr (is_same<T, f32, f64, f128>) {
            if (math::fcmp(unexpected, actual) == 0) {
                fail(std::format("Expected not {}, but got {}. {}", unexpected, actual, std::forward<CString>(message)), loc);
            }
        } else {
            if (unexpected == actual) {
                fail(std::format("Expected not {}, but got {}. {}", unexpected, actual, std::forward<CString>(message)), loc);
            }
        }
    }

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

private:
    static void fail(const CString& failureMessage, std::source_location loc) {
        throw assertion_failed_exception("{}", loc, failureMessage);
    }
};

} // namespace my::test

#endif // ASSERTIONS_HPP