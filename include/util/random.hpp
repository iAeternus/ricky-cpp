/**
 * @brief 随机数类
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "vec.hpp"

#include <random>
#include <mutex>

namespace my::util {

/**
 * @class Random
 * @brief 随机数类
 * @details 单例类，支持生成多种不同类型的随机数
 */
class Random final : public Object<Random> {
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
     * @brief 生成随机数
     * @param min 区间下界，包含
     * @param max 区间上界，若为整数则包含，若为浮点数则不包含
     * @return 随机数
     */
    template <typename T>
    T next(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) {
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                      "next() only supports integral and floating-point types");

        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(generator_);
        } else {
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(generator_);
        }
    }

    /**
     * @brief 生成指定长度的随机字符串，只包含a-z A-Z 0-9
     * @param len 字符串长度
     * @return 随机字符串
     */
    CString next_str(const usize len) {
        static const auto characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"_cs;
        CString result(len);
        std::uniform_int_distribution<usize> distribution(0, characters.size() - 1);

        for (usize i = 0; i < len; ++i) {
            result[i] = characters[distribution(generator_)];
        }

        return result;
    }

    /**
     * @brief 生成n个和为定值sum的均匀分布非负整数
     * TODO n 与 sum 接近时存在死循环bug
     */
    Vec<i32> generate_uniform_sum_numbers(const i32 n, i32 sum) {
        Vec<i32> numbers;
        if (n <= 0 || sum < 0) {
            return numbers;
        }

        if (sum == 0) {
            numbers.resize(n);
            return numbers;
        }
        if (n == 1) {
            numbers.push(sum);
            return numbers;
        }

        // 总元素数为 sum + n - 1（包括隔板和球）
        const i32 total_elements = sum + n - 1;
        Vec<i32> elements;

        // 创建数组 [1, 2, ..., total_elements]
        for (i32 i = 1; i <= total_elements; ++i) {
            elements.push(i);
        }

        std::shuffle(elements.begin(), elements.end(), generator_);

        // 选择前 n-1 个元素作为隔板位置
        Vec<i32> partitions;
        for (i32 i = 0; i < n - 1; ++i) {
            partitions.push(elements[i]);
        }

        // 排序隔板位置
        std::sort(partitions.begin(), partitions.end());

        // 添加前导0和后导sum + n
        Vec<i32> board = {0};
        board.extend(partitions);
        board.push(sum + n);

        // 计算相邻元素的差值，并减去1得到最终结果
        for (usize i = 1; i < board.size(); ++i) {
            const i32 diff = board[i] - board[i - 1];
            numbers.push(diff - 1);
        }

        return numbers;
    }

private:
    explicit Random(const u32 seed) :
            generator_(seed) {}

    std::mt19937 generator_;
    static Random* instance_;
};

inline Random* Random::instance_ = nullptr;

} // namespace my::util

#endif // RANDOM_HPP