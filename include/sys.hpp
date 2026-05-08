/**
 * @brief 平台宏定义
 * @author Ricky
 * @date 2026/5/8
 * @version 1.0
 */
#ifndef SYS_HPP
#define SYS_HPP

namespace my {

/**
 * @brief Windows
 */
#if defined(_WIN32) || defined(_WIN64)
#define RICKY_WIN 1
#else
#define RICKY_WIN 0
#endif

/**
 * @brief Linux
 */
#if defined(__linux__)
#define RICKY_LINUX 1
#else
#define RICKY_LINUX 0
#endif

/**
 * @brief MacOS
 */
#if defined(__APPLE__)
#define RICKY_MAC 1
#else
#define RICKY_MAC 0
#endif

} // namespace my

#endif // SYS_HPP