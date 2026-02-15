/**
 * @brief 字符串算法工具
 * @author Ricky
 * @date 2025/8/19
 * @version 1.0
 */

#ifndef STRING_ALGORITHM_HPP
#define STRING_ALGORITHM_HPP

#include "vec.hpp"

namespace my::util {

/**
 * @class StringAlgorithm
 * @brief 字符串算法工具类
 */
class StringAlgorithm {
public:
    /**
     * @brief KMP算法，查找模式串的第一个匹配位置
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     * @param txt_begin 文本串首迭代器
     * @param txt_end 文本串尾后迭代器
     * @param pat_begin 模式串首迭代器
     * @param pat_end 模式串尾后迭代器
     * @return 模式串的第一个匹配位置，未找到返回npos
     */
    template <std::input_iterator Iter1, std::input_iterator Iter2>
    static usize kmp_find(Iter1 txt_begin, Iter1 txt_end, Iter2 pat_begin, Iter2 pat_end) {
        if (is_empty(pat_begin, pat_end)) return npos;
        const usize p_size = length(pat_begin, pat_end);
        const auto next = get_next(pat_begin, pat_end);

        usize j = 0;
        for (auto it = txt_begin; it != txt_end; ++it) {
            // 失配，j按照next回跳
            while (j > 0 && *it != at(pat_begin, j)) {
                j = next[j - 1];
            }
            j += *it == at(pat_begin, j); // 匹配，j前进
            // 模式串匹配完，返回文本串匹配起点
            if (j == p_size) {
                return std::distance(txt_begin, it - p_size + 1);
            }
        }
        return npos;
    }

    /**
     * @brief 查找模式串的所有匹配位置
     * @note KMP算法，时间复杂度 O(n + m)，n为文本串的长度
     * @param txt_begin 文本串首迭代器
     * @param txt_end 文本串尾后迭代器
     * @param pat_begin 模式串首迭代器
     * @param pat_end 模式串尾后迭代器
     * @return 所有匹配位置
     */
    template <std::input_iterator Iter1, std::input_iterator Iter2>
    static Vec<usize> kmp_find_all(Iter1 txt_begin, Iter1 txt_end, Iter2 pat_begin, Iter2 pat_end) {
        Vec<usize> res;
        if (is_empty(pat_begin, pat_end)) return res;
        const usize p_size = length(pat_begin, pat_end);
        const auto next = get_next(pat_begin, pat_end);

        usize j = 0;
        for (auto it = txt_begin; it != txt_end; ++it) {
            // 失配，j按照next回跳
            while (j > 0 && *it != at(pat_begin, j)) {
                j = next[j - 1];
            }
            j += *it == at(pat_begin, j); // 匹配，j前进
            // 模式串匹配完，返回文本串匹配起点
            if (j == p_size) {
                res.push(std::distance(txt_begin, it - p_size + 1));
                j = next[j - 1];
            }
        }
        return res;
    }

private:
    /**
     * @brief 下标访问
     * @param begin 首迭代器
     * @param idx 下标
     */
    template <std::input_iterator Iter>
    static decltype(auto) at(Iter begin, usize idx) {
        return *(begin + idx);
    }

    /**
     * @brief 判空
     * @return true=空 false=不空
     */
    template <std::input_iterator Iter>
    static bool is_empty(Iter begin, Iter end) {
        return end == begin;
    }

    /**
     * @brief 获取字符串长度
     */
    template <std::input_iterator Iter>
    static usize length(Iter begin, Iter end) {
        return std::distance(begin, end);
    }

    /**
     * @brief KMP辅助函数，求next数组
     * @param pat_begin 模式串首迭代器
     * @param pat_end 模式串尾后迭代器
     * @note next[i]: 模式串[0, i)中最长相等前后缀的长度为next[i]
     * @note 时间复杂度为 O(m)，m为模式串的长度
     */
    template <std::input_iterator Iter>
    static Vec<usize> get_next(Iter pat_begin, Iter pat_end) {
        const usize p_size = length(pat_begin, pat_end);
        Vec<usize> next(p_size, 0);
        for (usize i = 1, j = 0; i < p_size; ++i) {
            // 失配，j按照next数组回跳
            while (j > 0 && at(pat_begin, i) != at(pat_begin, j)) {
                j = next[j - 1];
            }
            j += at(pat_begin, i) == at(pat_begin, j); // 匹配，j前进
            next[i] = j;
        }
        return next;
    }
};

} // namespace my::util

#endif // STRING_ALGORITHM_HPP
