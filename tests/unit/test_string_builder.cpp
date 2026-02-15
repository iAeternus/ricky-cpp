#include "test_string_builder.hpp"
#include "str_builder.hpp"
#include "ricky_test.hpp"

namespace my::test::test_string_builder {

void it_works() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append("aaa"_s).append("bbb").append("ccc").append("你好呀"_s).append('R');

    // Then
    Assertions::assertEquals("aaabbbccc你好呀R"_s, sb.build());
    Assertions::assertEquals("aaabbbccc你好呀R"_s, sb.build_move());
}

void should_append_format_string() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append_format("Case {}#: ", 1).append_format("{}+{}={}", 1, 1, 2);

    // Then
    Assertions::assertEquals("Case 1#: 1+1=2"_s, sb.build());
}

void should_append_n() {
    // Given
    util::StringBuilder sb;

    // When
    sb.append_n(util::CodePoint{"我"}, 10);

    // Then
    Assertions::assertEquals("我我我我我我我我我我"_s, sb.build());
}

void should_append_array() {
    // Given
    util::StringBuilder sb;
    const util::CodePoint<> cps[] = {'a', util::CodePoint{"我"}, 'b', 'c'};

    // When
    sb.append_array(cps, std::size(cps));

    // Then
    Assertions::assertEquals("a我bc"_s, sb.build());
}

void should_find() {
    // Given
    util::StringBuilder sb;
    sb.append("abcdef").append("\r\n\r\r").append("defghi");

    // When
    auto pos = sb.find("\r\n\r\r"_s);

    // Then
    Assertions::assertEquals(6ull, pos);
}

GROUP_NAME("test_string_builder")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_append_format_string),
    UNIT_TEST_ITEM(should_append_n),
    UNIT_TEST_ITEM(should_append_array),
    UNIT_TEST_ITEM(should_find))

} // namespace my::test::test_string_builder