#ifndef TEST_STRING_BUILDER_HPP
#define TEST_STRING_BUILDER_HPP

#include "ricky_test.hpp"
#include "StringBuilder.hpp"

namespace my::test::test_string_builder {

fn it_works = []() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append("aaa").append("bbb"_s).append("ccc").append("你好"_s).append('R');

    // Then
    Assertions::assertEquals("aaabbbccc你好R"_s, sb.str());
};

fn test_string_builder() {
    UnitTestGroup group{"test_string_builder"};

    group.addTest("it_works", it_works);

    group.startAll();
}

constexpr i32 N = 1e6;

fn speed_of_string_builder_append_string = []() {
    util::StringBuilder sb;
    for (usize i = 0; i < N; ++i) {
        sb.append("abcdef");
    }
    auto str = sb.str();
    Assertions::assertEquals(N * 6, str.size());
};

fn speed_of_std_string_splicing = []() {
    std::string str;
    for (usize i = 0; i < N; ++i) {
        str += "abcdef";
    }
    Assertions::assertEquals(N * 6, str.length());
};

fn test_string_builder_speed() {
    UnitTestGroup group{"test_string_builder_speed"};

    group.addTest("speed_of_string_builder_append_string", speed_of_string_builder_append_string);
    group.addTest("speed_of_std_string_splicing", speed_of_std_string_splicing);

    group.startAll();
}

} // namespace my::test::test_string_builder

#endif // TEST_STRING_BUILDER_HPP