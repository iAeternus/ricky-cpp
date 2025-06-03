#ifndef TEST_STRING_BUILDER_HPP
#define TEST_STRING_BUILDER_HPP

#include "ricky_test.hpp"
#include "Assertions.hpp"
#include "StringBuilder.hpp"

namespace my::test::test_string_builder {

fn it_works = []() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append("aaa").append("bbb"_s).append("ccc").append("你好"_s).append('R');

    // Then
    Assertions::assertEquals("aaabbbccc你好R"_s, sb.build());
};

fn should_append_format_string = []() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append_format("Case {}#: ", 1).append_format("{}+{}={}", 1, 1, 2);

    // Then
    Assertions::assertEquals("Case 1#: 1+1=2"_s, sb.build());
};

fn test_string_builder() {
    UnitTestGroup group{"test_string_builder"};

    group.addTest("it_works", it_works);
    group.addTest("should_append_format_string", should_append_format_string);

    group.startAll();
}

constexpr i32 N = 1e6;

fn speed_of_string_builder_append_string = []() {
    util::StringBuilder sb;
    for (usize i = 0; i < N; ++i) {
        sb.append("abcdef");
    }
    auto str = sb.build();
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