/**
 * @brief 随机数类
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "Vec.hpp"

#include <random>
#include <limits>
#include <mutex>
#include <type_traits>

namespace my::util {

class Random : public Object<Random> {
public:
    using Self = Random;
    using Super = Object<Self>;

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
    CString next_str(usize len) {
        static const CString characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"_cs;
        CString result(len);
        std::uniform_int_distribution<usize> distribution(0, characters.size() - 1);

        for (usize i = 0; i < len; ++i) {
            result[i] = characters[distribution(generator_)];
        }

        return result;
    }

    /**
     * @brief 生成n个和为定值sum的均匀分布非负整数
     * @note TODO n 与 sum 接近时存在死循环bug
     */
    Vec<i32> generate_uniform_sum_numbers(i32 n, i32 sum) {
        Vec<i32> numbers;
        if (n <= 0 || sum < 0) {
            return numbers;
        }

        if (sum == 0) {
            numbers.resize(n);
            return numbers;
        }
        if (n == 1) {
            numbers.append(sum);
            return numbers;
        }

        // 总元素数为 sum + n - 1（包括隔板和球）
        i32 total_elements = sum + n - 1;
        Vec<i32> elements;

        // 创建数组 [1, 2, ..., total_elements]
        for (i32 i = 1; i <= total_elements; ++i) {
            elements.append(i);
        }

        std::shuffle(elements.begin(), elements.end(), generator_);

        // 选择前 n-1 个元素作为隔板位置
        Vec<i32> partitions;
        for (i32 i = 0; i < n - 1; ++i) {
            partitions.append(elements[i]);
        }

        // 排序隔板位置
        std::sort(partitions.begin(), partitions.end());

        // 添加前导0和后导sum + n
        Vec<i32> board = {0};
        board.extend(partitions);
        board.append(sum + n);

        // 计算相邻元素的差值，并减去1得到最终结果
        for (usize i = 1; i < board.size(); ++i) {
            i32 diff = board[i] - board[i - 1];
            numbers.append(diff - 1);
        }

        return numbers;
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