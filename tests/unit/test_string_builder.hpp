#ifndef TEST_STRING_BUILDER_HPP
#define TEST_STRING_BUILDER_HPP

namespace my::test::test_string_builder {

void it_works();
void should_append_format_string();
void should_append_n();
void should_append_array();
void should_find();

void speed_of_string_builder_append_string();
void speed_of_std_string_append_string();
void test_string_builder_speed();

} // namespace my::test::test_string_builder

#endif // TEST_STRING_BUILDER_HPP