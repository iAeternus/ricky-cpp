#ifndef TEST_STRING_BUILDER_HPP
#define TEST_STRING_BUILDER_HPP

#include "ricky_test.hpp"
#include "str_builder.hpp"

namespace my::test::test_string_builder {

auto it_works = []() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append("aaa"_s).append("bbb").append("ccc").append("你好呀"_s).append('R');

    // Then
    Assertions::assertEquals("aaabbbccc你好呀R"_s, sb.build());
    Assertions::assertEquals("aaabbbccc你好呀R"_s, sb.build_move());
};

auto should_append_format_string = []() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append_format("Case {}#: ", 1).append_format("{}+{}={}", 1, 1, 2);

    // Then
    Assertions::assertEquals("Case 1#: 1+1=2"_s, sb.build());
};

auto should_append_n = []() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append_n(util::CodePoint{"我"}, 10);

    // Then
    Assertions::assertEquals("我我我我我我我我我我"_s, sb.build());
};

auto should_append_array = []() {
    // Given
    util::StringBuilder sb;
    const util::CodePoint<> cps[] = {'a', util::CodePoint{"我"}, 'b', 'c'};

    // When
    sb.append_array(cps, std::size(cps));

    // Then
    Assertions::assertEquals("a我bc"_s, sb.build());
};

auto should_find = []() {
    // Given
    util::StringBuilder sb;
    sb.append("abcdef").append("\r\n\r\r").append("defghi");

    // When
    auto pos = sb.find("\r\n\r\r"_s);

    // Then
    Assertions::assertEquals(6, pos);
};

auto test_string_builder() {
    UnitTestGroup group{"test_string_builder"};

    group.addTest("it_works", it_works);
    group.addTest("should_append_format_string", should_append_format_string);
    group.addTest("should_append_n", should_append_n);
    group.addTest("should_append_array", should_append_array);
    group.addTest("should_find", should_find);

    group.startAll();
}

constexpr i32 N = 1e5;

auto speed_of_string_builder_append_string = []() {
    util::StringBuilder sb;
    for (usize i = 0; i < N; ++i) {
        sb.append("abcdef");
    }
    auto str = sb.build();
    Assertions::assertEquals(N * 6, str.size());
};

auto speed_of_std_string_append_string = []() {
    std::string str;
    for (usize i = 0; i < N; ++i) {
        str += "abcdef";
    }
    Assertions::assertEquals(N * 6, str.length());
};

auto test_string_builder_speed() {
    UnitTestGroup group{"test_string_builder_speed"};

    group.addTest("speed_of_string_builder_append_string", speed_of_string_builder_append_string);
    group.addTest("speed_of_std_string_append_string", speed_of_std_string_append_string);

    group.startAll();
}

} // namespace my::test::test_string_builder

#endif // TEST_STRING_BUILDER_HPP