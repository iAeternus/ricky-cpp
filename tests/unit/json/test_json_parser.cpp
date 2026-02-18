#include "test_json_parser.hpp"
#include "printer.hpp"
#include "json_parser.hpp"
#include "ricky_test.hpp"

namespace my::test::test_json_parser {

void should_parse() {
    // Given
    util::String s = R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3}], "other": null, "bool": true })";

    // When
    auto json = json::parse_json(s);

    // Then
    Assertions::assertTrue(json.contains("array"));
    Assertions::assertTrue(json.contains("other"));
    Assertions::assertTrue(json.contains("bool"));
    Assertions::assertEquals(7ULL, json["array"].size());
    Assertions::assertTrue(json["other"].is_null());
    Assertions::assertTrue(json["bool"].into<bool>());

    // When
    auto json_str = json.dump();
    auto json2 = json::parse_json(json_str);

    // Then
    Assertions::assertEquals(7ULL, json2["array"].size());
    Assertions::assertTrue(json2["other"].is_null());
    Assertions::assertTrue(json2["bool"].into<bool>());
}

void should_fail_to_parse_if_json_str_is_empty() {
    // Given
    util::String s = "   ";

    Assertions::assertThrows("Empty json input", [&]() {
        json::parse_json(s);
    });
}

void should_dump() {
    // Given
    util::String s = R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3}], "other": null, "bool": true })";

    // When
    auto res = json::parse_json(s).dump(2);
    auto res2 = json::parse_json(s).dump(4);

    // Then
    io::println(res);
    io::println("---------------------------------");
    io::println(res2);
}

void should_parse_numbers() {
    auto json1 = json::parse_json("123"_s);
    auto json2 = json::parse_json("-42"_s);
    auto json3 = json::parse_json("3.1415"_s);
    auto json4 = json::parse_json("1e3"_s);
    auto json5 = json::parse_json("-2.5E-2"_s);

    Assertions::assertEquals(123LL, json1.into<i64>());
    Assertions::assertEquals(-42LL, json2.into<i64>());
    Assertions::assertTrue(json3.is<f64>());
    Assertions::assertTrue(json4.is<f64>());
    Assertions::assertTrue(json5.is<f64>());
}

void should_parse_string_escapes() {
    auto json1 = json::parse_json(R"("a\"b\\c\/d\b\f\n\r\t")"_s);
    auto s = json1.into<util::String>();
    Assertions::assertTrue(s.find(util::CodePoint{'\"'}) != npos);
    Assertions::assertTrue(s.find(util::CodePoint{'\\'}) != npos);
}

void should_parse_unicode_escape() {
    auto json1 = json::parse_json(R"("\u4F60\u597D")"_s);
    Assertions::assertEquals("你好"_s, json1.into<util::String>());
}

void should_parse_nested() {
    util::String s = R"({"a":[{"b":1}, {"c":[true, false, null]}], "d":{"e":"x"}})"_s;
    auto json = json::parse_json(s);
    Assertions::assertEquals(2ULL, json["a"].size());
    Assertions::assertEquals(1LL, json["a"][0]["b"].into<i64>());
    Assertions::assertTrue(json["a"][1]["c"][0].into<bool>());
    Assertions::assertTrue(json["a"][1]["c"][2].is<json::JsonType::JsonNull>());
    Assertions::assertEquals("x"_s, json["d"]["e"].into<util::String>());
}

void should_fail_invalid_json() {
    Assertions::assertThrows("Invalid json value", [&]() {
        json::parse_json("@"_s);
    });
    Assertions::assertThrows("Expected ',' or ']' in array", [&]() {
        json::parse_json("[1 2]"_s);
    });
    Assertions::assertThrows("Expected ',' or '}' in object", [&]() {
        json::parse_json(R"({"a":1 "b":2})"_s);
    });
    Assertions::assertThrows("Unterminated string", [&]() {
        json::parse_json("\"abc"_s);
    });
}

GROUP_NAME("test_json_parser")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_parse),
    UNIT_TEST_ITEM(should_fail_to_parse_if_json_str_is_empty),
    UNIT_TEST_ITEM(should_dump),
    UNIT_TEST_ITEM(should_parse_numbers),
    UNIT_TEST_ITEM(should_parse_string_escapes),
    UNIT_TEST_ITEM(should_parse_unicode_escape),
    UNIT_TEST_ITEM(should_parse_nested),
    UNIT_TEST_ITEM(should_fail_invalid_json))

} // namespace my::test::test_json_parser