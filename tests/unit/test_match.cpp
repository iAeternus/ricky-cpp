#include "unit/test_match.hpp"

#include "match.hpp"
#include "ricky_test.hpp"

#include "test/test_registry.hpp"

namespace my::test::test_match {

void it_works() {
    // Given
    std::variant<std::string, float> v = 0.1f;

    // When
    auto res = match(v, [](const std::string& s) {
        Assertions::assertEquals("abc", s);
        return s; }, [](float d) {
        Assertions::assertEquals(0.1f, d);
        return std::to_string(d); });

    // Then
    Assertions::assertEquals("0.100000", res);

    // When
    v = "abc";
    auto res2 = match(v, [](const std::string& s) {
        Assertions::assertEquals("abc", s);
        return s; }, [](float d) {
        Assertions::assertEquals(0.1f, d);
        return std::to_string(d); });

    // Then
    Assertions::assertEquals("abc", res2);
}

void test_match() {
    UnitTestGroup group{"test_match"};

    group.addTest("it_works", it_works);

    group.startAll();
}

GROUP_NAME("test_match")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works))
} // namespace my::test::test_match