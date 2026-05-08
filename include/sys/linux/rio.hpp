/**
 * @brief CSAPP RIO 库（Robust IO）
 * @author Ricky
 * @date 2026/5/8
 * @version 1.0
 */
#ifndef RIO_HPP
#define RIO_HPP

#include <cstddef>
#include <sys/types.h>

namespace my::sys {

/**
 * @brief 无缓冲IO
 */
ssize_t rio_readn(int fd, void* usrbuf, size_t n);
ssize_t rio_writen(int fd, void* usebuf, size_t n);

/**
 * @brief 带缓冲ID
 */
// TODO

} // namespace my::sys

#endif // RIO_HPP