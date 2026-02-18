#ifndef TEST_JSON_PARSER_HPP
#define TEST_JSON_PARSER_HPP

namespace my::test::test_json_parser {

void should_parse();
void should_fail_to_parse_if_json_str_is_empty();
void should_dump();
void should_parse_numbers();
void should_parse_string_escapes();
void should_parse_unicode_escape();
void should_parse_nested();
void should_fail_invalid_json();

} // namespace my::test::test_json_parser

#endif // TEST_JSON_PARSER_HPP