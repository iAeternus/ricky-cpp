#ifndef TEST_JSON_PARSER_HPP
#define TEST_JSON_PARSER_HPP

#include "ricky_test.hpp"
#include "JsonParser.hpp"

namespace my::test::test_json_parser {

/**
 * TODO 不知道为什么，dict的结尾不加逗号就报错
 */
auto should_parse = []() {
    // Given
    using namespace my::util;
    util::String s = R"({ "array": [1, 2, "3", 4, 5.6, ["a", "b", "c"], {"d": 1, "e": 2, "f": 3,}], "other": null, "bool": true, })";

    // When
    auto json = json::JsonParser::parse(s);

    // Then
    io::print(json);
    io::print(json.size());
    io::print(json["array"_s]);
    io::print(json["other"_s]);
    io::print(json["bool"_s]);

    // When
    auto jsonStr = json::JsonParser::parse(json);

    // Then
    io::print(jsonStr);
};

void test_json_parser() {
    UnitTestGroup group{"test_json_parser"};

    group.addTest("should_parse", should_parse);

    group.startAll();
}

} // namespace my::test::test_json_parser

#endif // TEST_JSON_PARSER_HPP