/**
 * @brief 随机数类
 * @author Ricky
 * @date 2024/12/19
 * @version 1.0
 */
#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "Object.hpp"

#include <random>
#include <limits>

namespace my::util {

class Random : public Object<Random> {
    using self = Random;
    using super = Object<self>;

public:
    Random(unsigned int seed = std::random_device{}()) :
            generator_(seed) {}

    /**
     * @brief 定点随机数生成区间为左闭右闭区间 [min, max]
     */
    u8 nextU8(u8 min = 0, u8 max = std::numeric_limits<u8>::max()) {
        std::uniform_int_distribution<u8> distribution(min, max);
        return distribution(generator_);
    }

    u16 nextU16(u16 min = 0, u16 max = std::numeric_limits<u16>::max()) {
        std::uniform_int_distribution<u16> distribution(min, max);
        return distribution(generator_);
    }

    u32 nextU32(u32 min = 0, u32 max = std::numeric_limits<u32>::max()) {
        std::uniform_int_distribution<u32> distribution(min, max);
        return distribution(generator_);
    }

    u64 nextU64(u64 min = 0, u64 max = std::numeric_limits<u64>::max()) {
        std::uniform_int_distribution<u64> distribution(min, max);
        return distribution(generator_);
    }

    i8 nextI8(i8 min = std::numeric_limits<i8>::min(), i8 max = std::numeric_limits<i8>::max()) {
        std::uniform_int_distribution<i8> distribution(min, max);
        return distribution(generator_);
    }

    i16 nextI16(i16 min = std::numeric_limits<i16>::min(), i16 max = std::numeric_limits<i16>::max()) {
        std::uniform_int_distribution<i16> distribution(min, max);
        return distribution(generator_);
    }

    i32 nextI32(i32 min = std::numeric_limits<i32>::min(), i32 max = std::numeric_limits<i32>::max()) {
        std::uniform_int_distribution<i32> distribution(min, max);
        return distribution(generator_);
    }

    i64 nextI64(i64 min = std::numeric_limits<i64>::min(), i64 max = std::numeric_limits<i64>::max()) {
        std::uniform_int_distribution<i64> distribution(min, max);
        return distribution(generator_);
    }

    /**
     * @brief 浮点随机数生成区间为左闭右开区间 [min, max)
     */
    f32 nextFloat(f32 min = std::numeric_limits<f32>::min(), f32 max = std::numeric_limits<f32>::max()) {
        std::uniform_real_distribution<f32> distribution(min, max);
        return distribution(generator_);
    }

    f64 nextDouble(f64 min = std::numeric_limits<f64>::min(), f64 max = std::numeric_limits<f64>::max()) {
        std::uniform_real_distribution<f64> distribution(min, max);
        return distribution(generator_);
    }

    CString next(c_size len) {
        static const CString characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"_cs;
        CString result{len};
        std::uniform_int_distribution<c_size> distribution(0, characters.size() - 1);

        for (c_size i = 0; i < len; ++i) {
            result[i] = characters[distribution(generator_)];
        }

        return result;
    }

private:
    std::mt19937 generator_;
};

static Random rnd{};

} // namespace my::util

#endif // RANDOM_HPP