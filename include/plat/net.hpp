#ifndef PLAT_NET_HPP
#define PLAT_NET_HPP

#include "str.hpp"
#include "my_types.hpp"

namespace my::plat::net {

enum class SocketFamily : u8 {
    Ipv4,
    Ipv6
};

enum class SocketType : u8 {
    Stream,
    Datagram
};

/**
 * @brief 不透明Socket句柄
 */
struct SocketHandle;

/**
 * @brief 网络栈初始化（Windows需要WSAStartup）
 */
void startup();

/**
 * @brief 网络栈清理（Windows需要WSACleanup）
 */
void cleanup();

/**
 * @brief 获取最近一次错误信息
 */
util::String last_error();

/**
 * @brief 创建套接字
 */
SocketHandle* create(SocketFamily family, SocketType type);

/**
 * @brief 检查套接字是否有效
 */
bool is_valid(SocketHandle* socket);

/**
 * @brief 关闭套接字
 */
void close(SocketHandle* socket);

/**
 * @brief 绑定地址
 */
void bind(SocketHandle* socket, const char* ip, u16 port);

/**
 * @brief 监听连接
 */
void listen(SocketHandle* socket, i32 backlog);

/**
 * @brief 接受连接（返回新句柄）
 */
SocketHandle* accept(SocketHandle* socket);

/**
 * @brief 连接到远端
 */
void connect(SocketHandle* socket, const char* ip, u16 port);

/**
 * @brief 发送原始字节
 */
usize send_bytes(SocketHandle* socket, const char* data, usize size, i32 flags);

/**
 * @brief 接收指定字节数
 */
util::String recv_bytes(SocketHandle* socket, usize size, i32 flags);

/**
 * @brief 设置超时（毫秒）
 * @param receive true=接收超时，false=发送超时
 */
void set_timeout_ms(SocketHandle* socket, u32 timeout_ms, bool receive);

/**
 * @brief 设置套接字选项
 */
void set_option(SocketHandle* socket, i32 level, i32 optname, const void* optval, u32 optlen);

} // namespace my::plat::net

#endif // PLAT_NET_HPP
