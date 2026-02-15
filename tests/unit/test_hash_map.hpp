#ifndef TEST_HASH_MAP_HPP
#define TEST_HASH_MAP_HPP

namespace my::test::test_hash_map {

void should_insert();
void should_get_or_default();
void should_fail_to_get_if_key_not_found();
void should_set_default();
void should_update();
void should_remove();
void should_operator();
void should_to_string();

void setup();
void speed_of_hash_map_count();
void speed_of_unordered_map_count();
void speed_of_hash_map_insert();
void speed_of_unordered_map_insert();
void test_hash_map_speed();

} // namespace my::test::test_hash_map

#endif // TEST_HASH_MAP_HPP