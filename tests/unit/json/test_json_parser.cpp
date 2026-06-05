#include "test_json_parser.hpp"
#include "printer.hpp"
#include "json_parser.hpp"
#include "ricky_test.hpp"

namespace my::test::test_json_parser {

void should_parse() {
    // Given
    str::String<> s(R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3}], "other": null, "bool": true })");

    // When
    auto json = json::parse_json(s);

    // Then
    Assertions::assert_true(json.contains("array"));
    Assertions::assert_true(json.contains("other"));
    Assertions::assert_true(json.contains("bool"));
    Assertions::assert_equals(7ULL, json["array"].size());
    Assertions::assert_true(json["other"].is_null());
    Assertions::assert_true(json["bool"].into<bool>());

    // When
    auto json_str = json.dump();
    auto json2 = json::parse_json(json_str);

    // Then
    Assertions::assert_equals(7ULL, json2["array"].size());
    Assertions::assert_true(json2["other"].is_null());
    Assertions::assert_true(json2["bool"].into<bool>());
}

void should_fail_to_parse_if_json_str_is_empty() {
    // Given
    str::String<> s("   ");

    Assertions::assert_throws("Empty json input", [&]() {
        json::parse_json(s);
    });
}

void should_dump() {
    // Given
    str::String<> s(R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3}], "other": null, "bool": true })");

    // When
    auto res = json::parse_json(s).dump(2);
    auto res2 = json::parse_json(s).dump(4);

    // Then
    io::println(res);
    io::println("---------------------------------");
    io::println(res2);
}

void should_parse_numbers() {
    auto json1 = json::parse_json("123");
    auto json2 = json::parse_json("-42");
    auto json3 = json::parse_json("3.1415");
    auto json4 = json::parse_json("1e3");
    auto json5 = json::parse_json("-2.5E-2");

    Assertions::assert_equals(123LL, json1.into<i64>());
    Assertions::assert_equals(-42LL, json2.into<i64>());
    Assertions::assert_true(json3.is<f64>());
    Assertions::assert_true(json4.is<f64>());
    Assertions::assert_true(json5.is<f64>());
}

void should_parse_string_escapes() {
    auto json1 = json::parse_json(R"("a\"b\\c\/d\b\f\n\r\t")");
    auto s = json1.into<str::String<>>();
    Assertions::assert_true(s.find(str::StringView("\"")).is_some());
    Assertions::assert_true(s.find(str::StringView("\\")).is_some());
}

void should_parse_unicode_escape() {
    auto json1 = json::parse_json(R"("\u4F60\u597D")");
    Assertions::assert_equals(str::String<>("你好"), json1.into<str::String<>>());
}

void should_parse_nested() {
    str::String<> s(R"({"a":[{"b":1}, {"c":[true, false, null]}], "d":{"e":"x"}})");
    auto json = json::parse_json(s);
    Assertions::assert_equals(2ULL, json["a"].size());
    Assertions::assert_equals(1LL, json["a"][0]["b"].into<i64>());
    Assertions::assert_true(json["a"][1]["c"][0].into<bool>());
    Assertions::assert_true(json["a"][1]["c"][2].is<json::JsonType::JsonNull>());
    Assertions::assert_equals(str::String<>("x"), json["d"]["e"].into<str::String<>>());
}

void should_fail_invalid_json() {
    Assertions::assert_throws("Invalid json value", [&]() {
        json::parse_json("@");
    });
    Assertions::assert_throws("Expected ',' or ']' in array", [&]() {
        json::parse_json("[1 2]");
    });
    Assertions::assert_throws("Expected comma or object end", [&]() {
        json::parse_json(R"({"a":1 "b":2})");
    });
    Assertions::assert_throws("Unterminated string", [&]() {
        json::parse_json("\"abc");
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
