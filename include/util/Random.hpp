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
#include <string>
#include <mutex>
#include <type_traits>

namespace my::util {

class Random : public Object<Random> {
    using self = Random;
    using super = Object<self>;

public:
    // 线程安全的单例模式
    static Random& instance() {
        static std::once_flag once;
        std::call_once(once, [] {
            instance_ = new Random(std::random_device{}());
        });
        return *instance_;
    }

    // 禁止拷贝和赋值
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    // 类型无关的 next 方法
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

    // nextStr 方法
    CString nextStr(c_size len) {
        static const CString characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"_cs;
        CString result(len);
        std::uniform_int_distribution<c_size> distribution(0, characters.size() - 1);

        for (c_size i = 0; i < len; ++i) {
            result[i] = characters[distribution(generator_)];
        }

        return result;
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