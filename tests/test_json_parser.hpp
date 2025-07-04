#ifndef TEST_JSON_PARSER_HPP
#define TEST_JSON_PARSER_HPP

#include "UnitTest.hpp"
#include "Assertions.hpp"
#include "JsonParser.hpp"

namespace my::test::test_json_parser {

// TODO 这个测试可能会导致程序崩溃
fn should_parse = []() {
    // Given
    util::String s = R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3}], "other": null, "bool": true })"_s;

    // When
    auto json = json::JsonParser::parse(s);

    // Then
    io::println(json);
    io::println(json.size());
    io::println(json["array"_s]);
    io::println(json["other"_s]);
    io::println(json["bool"_s]);

    // When
    auto jsonStr = json::JsonParser::parse(json);

    // Then
    io::println(jsonStr);
};

fn should_fail_to_parse_if_json_str_is_empty = []() {
    // Given
    util::String s = "   ";

    Assertions::assertThrows("invalid simple parse: ", [&]() {
        json::JsonParser::parse(s);
    });
};

fn test_json_parser() {
    UnitTestGroup group{"test_json_parser"};

    group.addTest("should_parse", should_parse);
    // group.addTest("should_fail_to_parse_if_json_str_is_empty", should_fail_to_parse_if_json_str_is_empty);

    group.startAll();
}

} // namespace my::test::test_json_parser

#endif // TEST_JSON_PARSER_HPP