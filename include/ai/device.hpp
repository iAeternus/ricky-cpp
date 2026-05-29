/**
 * @brief 设备
 * @author Ricky
 * @date 2026/5/29
 * @version 1.0
 */
#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "my_types.hpp"

namespace my::ai {

enum class Device : u8 {
    Cpu,
    Cuda,
};

} // namespace my::ai

#endif // DEVICE_HPP