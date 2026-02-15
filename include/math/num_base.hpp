/**
 * @brief 数字进制工具
 * @author Ricky
 * @date 2026/1/18
 * @version 1.0
 */
#ifndef NUM_BASE_HPP
#define NUM_BASE_HPP

#include <string>
#include <algorithm>
#include <climits>
#include "my_types.hpp"

namespace my::math {

/// 字符转数值，成功返回0，非法字符返回-1
inline i32 char_to_val(char c, i32& val) {
    i32 res = -1;
    if (c >= '0' && c <= '9') {
        val = c - '0';
        res = 0;
    } else if (c >= 'A' && c <= 'Z') {
        val = c - 'A' + 10;
        res = 0;
    } else if (c >= 'a' && c <= 'z') {
        val = c - 'a' + 10;
        res = 0;
    }
    return res;
}

/// 数值转字符，成功返回0，数值超出范围返回-1
inline i32 val_to_char(i32 val, char& c) {
    i32 res = -1;
    if (val >= 0 && val <= 9) {
        c = '0' + val;
        res = 0;
    } else if (val >= 10 && val <= 35) {
        c = 'A' + (val - 10);
        res = 0;
    }
    return res;
}

/// 检查溢出的乘法
inline i32 checked_mul(i64 a, i64 b, i64& res) {
    if (b != 0 && a > LLONG_MAX / b) {
        return -1;
    }
    res = a * b;
    return 0;
}

/// 检查溢出的加法
inline i32 checked_add(i64 a, i64 b, i64& res) {
    if (a > LLONG_MAX - b) {
        return -1;
    }
    res = a + b;
    return 0;
}

/**
 * 将源进制字符串转换为十进制值
 * @param num 源进制字符串
 * @param from_base 源进制
 * @param decimal_value 输出的十进制值
 * @param is_negative 是否为负数
 * @return 成功返回0，失败返回-1
 */
inline i32 convert_to_decimal(const std::string& num, i32 from_base, i64& decimal_val, bool& is_neg) {
    decimal_val = 0;
    is_neg = false;

    if (num.empty()) {
        return 0;
    }

    size_t start_idx = 0;
    if (num[0] == '-') {
        is_neg = true;
        start_idx = 1;
    }

    if (start_idx >= num.length()) {
        return -1; // 非法数字字符串
    }

    i64 power = 1;
    for (i32 i = static_cast<i32>(num.length()) - 1; i >= static_cast<i32>(start_idx); --i) {
        i32 digit_val;
        if (char_to_val(num[i], digit_val) < 0) {
            return -1; // 非法字符
        }

        if (digit_val >= from_base) {
            return -1; // 无效数字
        }

        i64 add_val;
        if (checked_mul(static_cast<i64>(digit_val), power, add_val) < 0) {
            return -1; // 乘法溢出
        }

        i64 new_decimal_val;
        if (checked_add(decimal_val, add_val, new_decimal_val) < 0) {
            return -1; // 加法溢出
        }
        decimal_val = new_decimal_val;

        // 仅在需要计算下一位时才更新power
        if (i > static_cast<i32>(start_idx)) {
            i64 new_power;
            if (checked_mul(power, from_base, new_power) < 0) {
                return -1; // 乘法溢出
            }
            power = new_power;
        }
    }

    if (is_neg) {
        if (decimal_val == LLONG_MIN) {
            // 特殊处理 LLONG_MIN
            // 这里需要特殊逻辑，因为 -LLONG_MIN 会溢出
            // TODO
        } else {
            decimal_val = -decimal_val; // 取绝对值
        }
    }

    return 0;
}

/// 十进制正数转目标进制字符串
static i32 convert_from_positive_decimal(u64 pos_decimal, i32 to_base, std::string& result) {
    result.clear();

    if (pos_decimal == 0) {
        result = "0";
        return 0;
    }

    while (pos_decimal > 0) {
        u64 r = pos_decimal % to_base;
        char digit_char;
        if (val_to_char(static_cast<i32>(r), digit_char) < 0) {
            return -1; // 数值超出范围
        }
        result.push_back(digit_char);
        pos_decimal /= to_base;
    }

    std::reverse(result.begin(), result.end());
    return 0;
}

/**
 * 将十进制值转换为目标进制字符串
 * @param decimal_value 十进制值
 * @param to_base 目标进制
 * @param result 输出的字符串（不包含符号，反向存储）
 * @param is_negative 是否为负数
 * @return 成功返回0，失败返回-1
 */
inline i32 convert_from_decimal(i64 decimal_val, i32 to_base, std::string& result, bool is_neg) {
    result.clear();

    if (decimal_val == 0) {
        result = "0";
        return 0;
    }

    u64 abs_val;
    if (is_neg) {
        if (decimal_val == LLONG_MIN) {
            // LLONG_MIN的绝对值无法用i64表示，需要特殊处理
            abs_val = static_cast<u64>(LLONG_MAX) + 1ULL;
        } else {
            abs_val = static_cast<u64>(-decimal_val);
        }
    } else {
        abs_val = static_cast<u64>(decimal_val);
    }

    return convert_from_positive_decimal(abs_val, to_base, result);
}

/**
 * 格式化结果字符串
 * @param num_str 转换后的数字字符串（不包含符号）
 * @param is_negative 是否为负数
 * @param width 目标位宽
 * @param result 格式化后的字符串
 */
static void format_result(const std::string& num_str, bool is_neg, i32 width, std::string& result) {
    result.clear();

    // 计算需要的前导零数量
    size_t num_len = num_str.length();
    size_t target_len = width > 0 ? static_cast<size_t>(width) : num_len;

    // 添加负号
    if (is_neg) {
        result.push_back('-');
    }

    // 添加前导零
    if (num_len < target_len) {
        size_t zeros_needed = target_len - num_len;
        result.append(zeros_needed, '0');
    }

    // 添加数字部分
    result.append(num_str);
}

/**
 * 任意进制转换
 * @param num 要转换的数字字符串
 * @param from_base 源进制（2-36）
 * @param to_base 目标进制（2-36）
 * @param width 位宽，若大于数字位宽则在高位补零
 * @param result 转换后的数字字符串
 * @return 成功返回0，错误返回-1
 */
inline i32 convert_base(const std::string& num, i32 from_base, i32 to_base, i32 width, std::string& result) {
    if (from_base < 2 || from_base > 36 || to_base < 2 || to_base > 36) {
        return -1;
    }

    if (num.empty()) {
        result = "0";
        return 0;
    }

    // 转换为十进制
    i64 decimal_val;
    bool is_neg;
    if (convert_to_decimal(num, from_base, decimal_val, is_neg) < 0) {
        return -1;
    }

    // 转换为目标进制
    std::string target_num;
    if (convert_from_decimal(decimal_val, to_base, target_num, is_neg) < 0) {
        return -1;
    }

    // 格式化结果
    format_result(target_num, is_neg, width, result);

    return 0;
}

/**
 * 任意进制转换，默认位宽
 */
inline i32 convert_base(const std::string& num, i32 from_base, i32 to_base, std::string& result) {
    return convert_base(num, from_base, to_base, 0, result);
}

} // namespace my::math

#endif // NUM_BASE_HPP