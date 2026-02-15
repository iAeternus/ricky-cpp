/**
 * @brief 测试工具
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include "vec.hpp"
#include "timer.hpp"
#include "printer.hpp"
#include "my_exception.hpp"

namespace my::test {

static io::ColorPrinter test_passed{stdout, color::Color::GREEN};
static io::ColorPrinter test_failed{stdout, color::Color::RED};

/**
 * 单元测试类
 */
class UnitTest : public Object<UnitTest> {
    using Self = UnitTest;

public:
    using TestCase = Runnable;

    UnitTest(const CString& displayName, const TestCase& testCase) :
            displayName_(displayName), testCase_(testCase) {}

    UnitTest(CString&& displayName, TestCase&& testCase) :
            displayName_(std::move(displayName)), testCase_(std::move(testCase)) {}

    ~UnitTest() = default;

    CString displayName() const {
        return displayName_;
    }

    bool start() {
        try {
            test_passed(std::format("[{}] Test passed in {}us", displayName_, timer_(testCase_)));
            return true;
        } catch (const Exception& ex) {
            test_failed(std::format("[{}] Test failed!\nCaused by:\n{}", displayName_, ex.what()));
            return false;
        } catch (...) {
            test_failed(std::format("[{}] Test failed! Unknown exception.", displayName_));
            return false;
        }
    }

private:
    CString displayName_;
    TestCase testCase_;
    util::Timer_us timer_;
};

/**
 * 单元测试组
 */
class UnitTestGroup : public Object<UnitTestGroup> {
public:
    UnitTestGroup(CString&& groupName) :
            groupName_(std::move(groupName)), failed_(0), group_() {}

    ~UnitTestGroup() = default;

    void setup(Runnable&& setup) {
        this->setup_ = setup;
    }

    void teardown(Runnable&& teardown) {
        this->teardown_ = teardown;
    }

    void before_each(Runnable&& before_each) {
        this->before_each_ = before_each;
    }

    void after_each(Runnable&& after_each) {
        this->after_each_ = after_each;
    }

    void addTest(CString&& displayName, Runnable&& testCase) {
        auto* item = new UnitTest{std::forward<CString>(displayName), std::forward<Runnable>(testCase)};
        group_.push(item);
    }

    void startAll() {
        io::println(std::format("================= {} =================", groupName_));
        if (setup_) setup_();
        for (const auto& it : group_) {
            if (before_each_) before_each_();
            if (!it->start()) {
                ++failed_;
            }
            if (after_each_) after_each_();
        }
        if (teardown_) teardown_();
        io::println(std::format("Total tests run: {}, Failures: {}", group_.len(), failed_));
        if (failed_ > 0) {
            throw assertion_failed_exception("UnitTestGroup {} failed: {}", groupName_, failed_);
        }
    }

private:
    CString groupName_;
    usize failed_;
    Runnable setup_;
    Runnable teardown_;
    Runnable before_each_;
    Runnable after_each_;
    util::Vec<UnitTest*> group_;
};

} // namespace my::test

#endif // TEST_UTILS_HPP
