/**
 * @brief UDP服务器、客户端
 * @author Ricky
 * @date 2025/7/13
 * @version 1.0
 */
#ifndef UDP_HPP
#define UDP_HPP

#include "Socket.hpp"

namespace my::net {

/**
 * @brief UDP服务器
 * @details 该类用于创建UDP服务器，监听指定端口并接收数据
 */
class UdpServer : public Object<UdpServer> {
public:
    using Self = UdpServer;

    /**
     * @brief 使用ip和端口构造UDP服务器
     * @param ip IP地址字符串
     * @param port 端口号
     * @param family 地址族，默认为AF_INET
     * @throws runtime_exception 如果创建套接字失败
     */
    UdpServer(const char* ip, u16 port, i32 family = AF_INET) :
            socket_(family, SOCK_DGRAM) {
        socket_.bind(ip, port); // 绑定到指定IP和端口
    }

    /**
     * @brief 发送数据到指定地址，需要用recvfrom接收
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param to 目标地址
     * @param flags 发送标志，默认为0
     * @throws runtime_exception 如果发送失败
     */
    void sendto(const char* data, usize size, const SockAddrIn& to, i32 flags = 0) const {
        return socket_.sendto(data, size, to, flags);
    }

    /**
     * @brief 接收数据
     * @param flags 接收标志，默认为0
     * @return 返回接收到的数据和发送方地址
     * @throws runtime_exception 如果接收失败
     */
    Pair<CString, SockAddrIn> recvfrom(i32 flags = 0) const {
        return socket_.recvfrom(flags);
    }

private:
    Socket socket_; // UDP套接字
};

/**
 * @brief UDP客户端
 * @details 该类用于创建UDP客户端，发送数据到指定服务器地址和端口
 */
class UdpClient : public Object<UdpClient> {
public:
    using Self = UdpClient;

    /**
     * @brief 使用地址族构造UDP客户端
     * @param family 地址族，默认为AF_INET
     * @throws runtime_exception 如果创建套接字失败
     */
    UdpClient(i32 family = AF_INET) :
            socket_(family, SOCK_DGRAM) {}

    /**
     * @brief 使用服务器地址和端口构造UDP客户端
     * @param server_ip 服务器IP地址字符串
     * @param server_port 服务器端口号
     * @param family 地址族，默认为AF_INET
     */
    UdpClient(const char* server_ip, u16 server_port, i32 family = AF_INET) :
            socket_(family, SOCK_DGRAM), server_addr_(server_ip, server_port) {}

    ~UdpClient() {
        sendto("", 0); // 发送空数据包以通知服务器关闭连接
    }

    /**
     * @brief 向服务器发送数据，需要用recvfrom接收
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param flags 发送标志，默认为0
     * @throws runtime_exception 如果发送失败
     */
    void sendto(const char* data, usize size, i32 flags = 0) const {
        if (server_addr_.get_socklen() == 0) {
            throw runtime_exception("server address is not set");
        }
        socket_.sendto(data, size, server_addr_, flags);
    }

    /**
     * @brief 接收数据
     * @param flags 接收标志，默认为0
     * @return 返回接收到的数据和发送方地址
     * @throws runtime_exception 如果接收失败
     */
    Pair<CString, SockAddrIn> recvfrom(i32 flags = 0) const {
        return socket_.recvfrom(flags);
    }

private:
    Socket socket_;          // UDP套接字
    SockAddrIn server_addr_; // 服务器地址
};

} // namespace my::net

#endif // UDP_HPP