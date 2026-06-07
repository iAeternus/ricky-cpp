/**
 * @brief 字符串算法：Two-Way 查找
 * @author Ricky
 * @date 2026/2/16
 * @version 1.0
 */
#ifndef STR_STRING_ALGORITHM_HPP
#define STR_STRING_ALGORITHM_HPP

#include "vec.hpp"
#include "option.hpp"

namespace my::str {

/**
 * @brief 使用 Two-Way 算法在文本中查找模式串
 * @param hay 文本串指针
 * @param hlen 文本串长度
 * @param pat 模式串指针
 * @param plen 模式串长度
 * @return 匹配位置的 Option，存在时返回 Some(index)，不存在返回 None
 * @note Two-Way 算法，时间复杂度 O(n + m)，空间复杂度 O(1)
 *       适用于任意字节序列的字符串查找
 */
Option<usize> twoway_find(const u8* hay, const usize hlen, const u8* pat, const usize plen);

/**
 * @brief 使用 KMP 算法在文本中查找模式串
 * @param hay 文本串指针
 * @param hlen 文本串长度
 * @param pat 模式串指针
 * @param plen 模式串长度
 * @return 匹配位置的 Option，存在时返回 Some(index)，不存在返回 None
 * @note KMP 算法，时间复杂度 O(n + m)，空间复杂度 O(m)
 *       适用于任意字节序列的字符串查找
 */
Option<usize> kmp_find(const u8* hay, const usize hlen, const u8* pat, const usize plen);

/**
 * @brief 使用 KMP 算法在文本中查找所有模式串
 * @param hay 文本串指针
 * @param hlen 文本串长度
 * @param pat 模式串指针
 * @param plen 模式串长度
 * @return 匹配位置的列表，若不存在返回空列表
 * @note KMP 算法，时间复杂度 O(n + m)，空间复杂度 O(m)
 *       适用于任意字节序列的字符串查找
 */
util::Vec<usize> kmp_find_all(const u8* hay, const usize hlen, const u8* pat, const usize plen);

} // namespace my::str

#endif // STR_STRING_ALGORITHM_HPP
