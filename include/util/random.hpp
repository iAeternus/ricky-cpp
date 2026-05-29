/**
 * @brief 随机数类
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "marker.hpp"
#include "vec.hpp"
#include "math_concepts.hpp"

#include <random>

namespace my::util {

class Random final : public Object<Random>, public NoCopy {
public:
    using Self = Random;
    using Engine = std::mt19937_64;

    /**
     * @brief 使用随机种子构造
     */
    Random();

    /**
     * @brief 使用指定种子构造
     * @param seed 随机种子
     */
    explicit Random(u64 seed);

    ~Random() = default;

    /**
     * @brief 获取线程局部随机数生成器
     * @return 随机数生成器
     */
    static Random& thread_local_rng();

    /**
     * @brief 设置随机种子
     * @param seed 随机种子
     */
    void seed(u64 seed);

    /**
     * @brief 生成随机值
     * @return 随机值
     *
     * 整数：
     * 在类型完整范围内均匀分布
     *
     * 浮点：
     * 在[0,1)内均匀分布
     */
    template <math::NumericType T>
    T gen();

    /**
     * @brief 生成指定范围随机值
     * @param min 下界，包含
     * @param max 上界，不包含
     * @return 随机值
     *
     * 整数：
     * [min,max)
     *
     * 浮点：
     * [min,max)
     */
    template <math::NumericType T>
    T gen_range(T min, T max);

    /**
     * @brief 生成正态分布随机数
     * @param mean 均值
     * @param stddev 标准差
     * @return 随机数
     */
    template <math::FloatingPointType T>
    T normal(T mean = static_cast<T>(0), T stddev = static_cast<T>(1));

    /**
     * @brief 生成伯努利分布随机数
     * @param p 为true的概率
     * @return 随机布尔值
     */
    bool bernoulli(f64 p);

    /**
     * @brief 生成随机字符串
     * @param len 字符串长度
     * @return 随机字符串
     *
     * 字符集：
     * a-z A-Z 0-9
     */
    CString string(usize len);

    /**
     * @brief 打乱序列
     * @param begin 起始迭代器
     * @param end 结束迭代器
     */
    template <typename Iter>
    void shuffle(Iter begin, Iter end);

    /**
     * @brief 随机生成n个和为sum的非负整数
     * @param n 整数个数
     * @param sum 总和
     * @return 随机整数数组
     *
     * 结果满足：
     *
     * result[i] >= 0
     * Σ(result[i]) == sum
     */
    util::Vec<i32> partition_nonnegative(i32 n, i32 sum);

private:
    Engine engine_;
};

template <math::NumericType T>
T Random::gen() {
    if constexpr (math::IntegerType<T>) {
        std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        return dist(engine_);
    } else {
        std::uniform_real_distribution<T> dist(static_cast<T>(0), static_cast<T>(1));
        return dist(engine_);
    }
}

template <math::NumericType T>
T Random::gen_range(T min, T max) {
    if (min >= max) {
        throw argument_exception("Random::gen_range requires min < max, but given min: {} max: {}", min, max);
    }

    if constexpr (math::IntegerType<T>) {
        std::uniform_int_distribution<T> dist(min, max - 1);
        return dist(engine_);
    } else {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(engine_);
    }
}

template <math::FloatingPointType T>
T Random::normal(T mean, T stddev) {
    std::normal_distribution<T> dist(mean, stddev);
    return dist(engine_);
}

template <typename Iter>
void Random::shuffle(Iter begin, Iter end) {
    std::shuffle(begin, end, engine_);
}

} // namespace my::util

#endif // RANDOM_HPP