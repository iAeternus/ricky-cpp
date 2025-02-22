/**
 * @brief 随机数类
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "DynArray.hpp"

#include <random>
#include <limits>
#include <string>
#include <mutex>
#include <type_traits>

namespace my::util {

class Random : public Object<Random> {
    using self = Random;
    using super = Object<self>;

public:
    static Random& instance() {
        static std::once_flag once;
        std::call_once(once, [] {
            instance_ = new Random(std::random_device{}());
        });
        return *instance_;
    }

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    /**
     * @brief 生成随机数，整数在闭区间内，浮点数在左闭右开区间内
     */
    template <typename T>
    T next(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) {
        static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value,
                      "next() only supports integral and floating-point types");

        if constexpr (std::is_integral<T>::value) {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(generator_);
        } else {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(generator_);
        }
    }

    /**
     * @brief 生成指定长度的随机字符串
     */
    CString nextStr(isize len) {
        static const CString characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"_cs;
        CString result(len);
        std::uniform_int_distribution<isize> distribution(0, characters.size() - 1);

        for (isize i = 0; i < len; ++i) {
            result[i] = characters[distribution(generator_)];
        }

        return result;
    }

    /**
     * @brief 生成n个和为定值sum的均匀分布非负整数
     */
    DynArray<i32> generate_uniform_sum_numbers(i32 n, i32 sum) {
        if (n <= 0 || sum < 0) {
            return {};
        }

        std::vector<i32> numbers;

        if (sum == 0) {
            numbers.resize(n, 0);
            return numbers;
        }
        if (n == 1) {
            numbers.push_back(sum);
            return numbers;
        }

        // 总元素数为 sum + n - 1（包括隔板和球）
        i32 total_elements = sum + n - 1;
        std::vector<i32> elements;

        // 创建数组 [1, 2, ..., total_elements]
        for (i32 i = 1; i <= total_elements; ++i) {
            elements.push_back(i);
        }

        std::shuffle(elements.begin(), elements.end(), generator_);

        // 选择前 n-1 个元素作为隔板位置
        std::vector<i32> partitions;
        for (i32 i = 0; i < n - 1; ++i) {
            partitions.push_back(elements[i]);
        }

        // 排序隔板位置
        std::sort(partitions.begin(), partitions.end());

        // 添加前导0和后导sum + n
        std::vector<i32> board = {0};
        board.insert(board.end(), partitions.begin(), partitions.end());
        board.push_back(sum + n);

        // 计算相邻元素的差值，并减去1得到最终结果
        for (size_t i = 1; i < board.size(); ++i) {
            i32 diff = board[i] - board[i - 1];
            numbers.push_back(diff - 1);
        }

        return DynArray<i32>(numbers);
    }

private:
    Random(u32 seed) :
            generator_(seed) {}

    std::mt19937 generator_;
    static Random* instance_;
};

Random* Random::instance_ = nullptr;

} // namespace my::util

#endif // RANDOM_HPP