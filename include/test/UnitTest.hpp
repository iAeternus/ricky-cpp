/**
 * @brief 测试工具
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "Object.hpp"
#include "CString.hpp"
#include "timer.hpp"
#include "raise_error.hpp"

#include <functional>
// #include <utility>

namespace my::test {

static io::ColorPrinter test_passed{stdout, io::Color::GREEN};
static io::ColorPrinter test_failed{stdout, io::Color::RED};

/**
 * 单元测试类
 */
class UnitTest : Object<UnitTest> {
    using self = UnitTest;

public:
    UnitTest(const CString& displayName, const std::function<void(void)>& testCase) :
            displayName_(displayName), testCase_(testCase) {}

    UnitTest(CString&& displayName, std::function<void(void)>&& testCase) :
            displayName_(std::forward<CString>(displayName)), testCase_(std::forward<std::function<void(void)>>(testCase)) {}

    CString displayName() const {
        return displayName_;
    }

    void start() {
        try {
            test_passed("[", displayName_, "] Test passed in ", timer_(testCase_), "ms");
        } catch (const std::runtime_error& re) {
            test_failed("[", displayName_, "] Test failed! Exception: ", re.what());
        } catch (...) {
            test_failed("[", displayName_, "] Test failed! Unknown exception.");
        }
    }

private:
    CString displayName_;
    std::function<void(void)> testCase_;
    util::Timer_ms timer_;
};

/**
 * 单元测试组
 */
class UnitTestGroup : Object<UnitTestGroup> {
    // TODO
};

} // namespace my::test

#endif // TEST_UTILS_HPP