#ifndef TEST_JSON_SERIALIZER_HPP
#define TEST_JSON_SERIALIZER_HPP

namespace my::test::test_json_serializer {

void it_works();
void should_roundtrip_basic_types();
void should_roundtrip_container_types();
void should_roundtrip_person();
void should_fail_missing_field();

} // namespace my::test::test_json_serializer

#endif // TEST_JSON_SERIALIZER_HPP