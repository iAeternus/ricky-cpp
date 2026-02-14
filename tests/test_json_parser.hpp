#ifndef TEST_JSON_PARSER_HPP
#define TEST_JSON_PARSER_HPP

#include "printer.hpp"
#include "ricky_test.hpp"
#include "json/json_parser.hpp"

namespace my::test::test_json_parser {

auto should_parse = []() {
    // Given
    util::String s = R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3}], "other": null, "bool": true })";

    // When
    auto json = json::parse_json(s);

    // Then
    io::println(json);
//    io::println(json.size());
    io::println(json["array"]);
    io::println(json["other"]);
    io::println(json["bool"]);

//    // When TODO
//    auto jsonStr = json::JsonParser::parse(json);
//
//    // Then
//    io::println(jsonStr);
};

auto should_fail_to_parse_if_json_str_is_empty = []() {
    // Given
    util::String s = "   ";

    Assertions::assertThrows("Invalid simple parse: ", [&]() {
        json::parse_json(s);
    });
};

auto should_dump = []() {
    // Given
    util::String s = R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3}], "other": null, "bool": true })";

    // When
    auto res = json::parse_json(s).dump(2);
    auto res2 = json::parse_json(s).dump(4);

    // Then
    io::println(res);
    io::println("---------------------------------");
    io::println(res2);
};

auto test_json_parser() {
    UnitTestGroup group{"test_json_parser"};

    group.addTest("should_parse", should_parse);
    group.addTest("should_fail_to_parse_if_json_str_is_empty", should_fail_to_parse_if_json_str_is_empty);
    group.addTest("should_dump", should_dump);

    group.startAll();
}

} // namespace my::test::test_json_parser

#endif // TEST_JSON_PARSER_HPP