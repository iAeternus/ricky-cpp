/**
 * @brief 设备
 * @author Ricky
 * @date 2026/5/29
 * @version 1.0
 */
#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "my_types.hpp"

namespace my::nn {

enum class Device : u8 {
    Cpu,
    Cuda,
};

} // namespace my::nn

#endif // DEVICE_HPP