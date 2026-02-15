#ifndef PLAT_PROCESS_HPP
#define PLAT_PROCESS_HPP

#include "my_types.hpp"

namespace my::plat::process {

/**
 * @brief 获取当前进程ID
 */
u32 pid();

/**
 * @brief 设置控制台为UTF-8输出（非Windows可为空实现）
 */
void set_console_utf8();

} // namespace my::plat::process

#endif // PLAT_PROCESS_HPP
