/**
 * @brief
 * @author Ricky
 * @date 2025/8/19
 * @version 1.0
 */

#ifndef TEST_STRING_ALGORITHM_HPP
#define TEST_STRING_ALGORITHM_HPP

#include "ricky_test.hpp"
#include "str.hpp"
#include "str_algorithm.hpp"

namespace my::test::test_string_algorithm {

auto test_kmp_find = []() {
    // Given
    util::String s = "abcdef";
    util::String t = "bcd";
    util::StringView v1(s);
    util::StringView v2(t);
    // CString cs = "abcdef";
    // CString ct = "bcd";

    // When
    auto res = util::StringAlgorithm::kmp_find(s.begin(), s.end(), t.begin(), t.end());
    auto res2 = util::StringAlgorithm::kmp_find(v1.begin(), v1.end(), v2.begin(), v2.end());
    // auto jt = util::StringAlgorithm::kmp_find(cs.begin(), cs.end(), ct.begin(), ct.end());

    // Then
    Assertions::assertEquals(1, res);
    Assertions::assertEquals(1, res2);
};

auto test_kmp_find_all = []() {
    // Given
    util::String s = "abcdebcdf";
    util::String t = "bcd";
    util::StringView v1(s);
    util::StringView v2(t);

    // When
    auto res = util::StringAlgorithm::kmp_find_all(s.begin(), s.end(), t.begin(), t.end());
    auto res2 = util::StringAlgorithm::kmp_find_all(v1.begin(), v1.end(), v2.begin(), v2.end());

    Assertions::assertEquals("[1,5]"_cs, res.__str__());
    Assertions::assertEquals("[1,5]"_cs, res2.__str__());
};

auto test_string_algorithm() {
    UnitTestGroup group{"test_string_algorithm"};

    group.addTest("should_find", test_kmp_find);
    group.addTest("test_kmp_find_all", test_kmp_find_all);

    group.startAll();
}

} // namespace my::test::test_string_algorithm

#endif // TEST_STRING_ALGORITHM_HPP
