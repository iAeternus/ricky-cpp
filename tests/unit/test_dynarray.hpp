#ifndef TEST_DYNARRAY_HPP
#define TEST_DYNARRAY_HPP

namespace my::test::test_dynarray {

void it_works();
void should_append();
void should_insert();
void should_pop();
void should_pop2();
void should_clear();
void should_to_array();
void should_extend();
void should_at();
void should_find();

void speed_of_dny_array_append_string();
void speed_of_std_vector_push_back_string();
void speed_of_dny_array_append_i32();
void speed_of_std_vector_push_back_i32();
void test_dynarray_speed();

} // namespace my::test::test_dynarray

#endif // TEST_DYNARRAY_HPP