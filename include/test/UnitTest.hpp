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
#include "DynArray.hpp"
#include "printer.hpp"

#include <functional>

namespace my::test {

static io::ColorPrinter test_passed{stdout, io::Color::GREEN};
static io::ColorPrinter test_failed{stdout, io::Color::RED};

/**
 * 单元测试类
 */
class UnitTest : public Object<UnitTest> {
    using self = UnitTest;

public:
    UnitTest(const CString& displayName, const std::function<void(void)>& testCase) :
            displayName_(displayName), testCase_(testCase) {}

    UnitTest(CString&& displayName, std::function<void(void)>&& testCase) :
            displayName_(std::move(displayName)), testCase_(std::move(testCase)) {}

    ~UnitTest() = default;

    CString displayName() const {
        return displayName_;
    }

    bool start() {
        try {
            test_passed(std::format("[{}] Test passed in {}ms", displayName_, timer_(testCase_)));
            return true;
        } catch (const std::runtime_error& re) {
            test_failed(std::format("[{}] Test failed! Exception: {}", displayName_, re.what()));
            return false;
        } catch (...) {
            test_failed(std::format("[{}] Test failed! Test failed! Unknown exception.", displayName_));
            return false;
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
class UnitTestGroup : public Object<UnitTestGroup> {
public:
    UnitTestGroup(CString&& groupName) :
            groupName_(std::move(groupName)), failed_(0), group_() {}

    ~UnitTestGroup() = default;

    void addTest(CString&& displayName, std::function<void(void)>&& testCase) {
        auto* item = new UnitTest{std::forward<CString>(displayName), std::forward<std::function<void(void)>>(testCase)};
        group_.append(item);
    }

    void startAll() {
        io::print(std::format("================= {} =================", groupName_));
        for(auto& it : group_) {
            if(!it->start()) {
                ++failed_;
            }
        }
        io::print(std::format("Total tests run: {}, Failures: {}", group_.size(), failed_));
    }

private:
    CString groupName_;
    c_size failed_;
    util::DynArray<UnitTest*> group_;
};

} // namespace my::test

#endif // TEST_UTILS_HPP