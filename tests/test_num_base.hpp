#ifndef TEST_NUM_BASE_HPP
#define TEST_NUM_BASE_HPP

#include "num_base.hpp"
#include "ricky_test.hpp"

namespace my::test::test_num_base {

// 基本二进制转十进制
auto basic_binary_to_decimal = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("1010", 2, 10, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("10", res);
};

// 带位宽的转换
auto conversion_with_width = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("1010", 2, 10, 6, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("000010", res);
};

// 十六进制转十进制
auto hex_to_decimal = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("FF", 16, 10, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("255", res);
};

// 十进制转十六进制
auto decimal_to_hex = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("255", 10, 16, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("FF", res);
};

// 负数转换
auto negative_conversion = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("-42", 10, 2, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("-101010", res);
};

// 带位宽的负数转换
auto negative_with_width = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("-42", 10, 2, 8, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("-00101010", res);
};

// 0的转换
auto zero_conversion = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("0", 10, 2, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("0", res);
};

// 空字符串
auto empty_string = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("", 10, 2, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("0", res);
};

// 大数转换 (LLONG_MAX的十六进制表示)
auto large_number = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("7FFFFFFFFFFFFFFF", 16, 10, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("9223372036854775807", res);
};

// LLONG_MIN转换
auto llmin_conversion = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("-8000000000000000", 16, 10, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("-9223372036854775808", res);
};

// 无效进制测试
auto invalid_base = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("1010", 1, 10, res);

    // Then
    Assertions::assert_true(ret < 0);
};

// 非法字符
auto invalid_characters = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("10G", 10, 2, res);

    // Then
    Assertions::assert_true(ret < 0);
};

// 数字超过进制范围
auto digit_exceeds_base = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("3A", 10, 2, res);

    // Then
    Assertions::assert_true(ret < 0);
};

// 二进制转八进制
auto binary_to_octal = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("110101", 2, 8, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("65", res);
};

// 十进制转三十六进制
auto decimal_to_base36 = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("12345", 10, 36, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("9IX", res);
};

// 不同进制间的转换
auto base8_to_base16 = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("777", 8, 16, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("1FF", res);
};

// 带位宽的十六进制转换
auto hex_with_width = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("FF", 16, 2, 16, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("0000000011111111", res);
};

// 溢出测试
auto overflow_test = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("10000000000000000000000000000000000000000000000000000000000000000", 2, 10, res);

    // Then
    Assertions::assert_true(ret < 0); // 应该溢出
};

// 仅负号的情况
auto only_minus_sign = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("-", 10, 2, res);

    // Then
    Assertions::assert_true(ret < 0); // 应该返回错误
};

// 大写和小写混合
auto mixed_case = []() {
    // Given
    std::string res;

    // When
    int ret = math::convert_base("FfAa", 16, 10, res);

    // Then
    Assertions::assert_true(ret >= 0);
    Assertions::assert_equals("65450", res); // 0xFFAA = 65450
};

void test_num_base() {
    UnitTestGroup group{"test_num_base"};

    // 添加所有测试用例
    group.addTest("basic_binary_to_decimal", basic_binary_to_decimal);
    group.addTest("conversion_with_width", conversion_with_width);
    group.addTest("hex_to_decimal", hex_to_decimal);
    group.addTest("decimal_to_hex", decimal_to_hex);
    group.addTest("negative_conversion", negative_conversion);
    group.addTest("negative_with_width", negative_with_width);
    group.addTest("zero_conversion", zero_conversion);
    group.addTest("empty_string", empty_string);
    group.addTest("large_number", large_number);
//    group.addTest("llmin_conversion", llmin_conversion); // TODO
    group.addTest("invalid_base", invalid_base);
    group.addTest("invalid_characters", invalid_characters);
    group.addTest("digit_exceeds_base", digit_exceeds_base);
    group.addTest("binary_to_octal", binary_to_octal);
    group.addTest("decimal_to_base36", decimal_to_base36);
    group.addTest("base8_to_base16", base8_to_base16);
    group.addTest("hex_with_width", hex_with_width);
    group.addTest("overflow_test", overflow_test);
    group.addTest("only_minus_sign", only_minus_sign);
    group.addTest("mixed_case", mixed_case);

    group.startAll();
}

} // namespace my::test::test_num_base

#endif // TEST_NUM_BASE_HPP