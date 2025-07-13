/**
 * @brief TCP服务器、客户端
 * @author Ricky
 * @date 2025/7/13
 * @version 1.0
 */
#ifndef TCP_HPP
#define TCP_HPP

#include "Socket.hpp"
#include "Vec.hpp"

namespace my::net {

/**
 * @brief TCP服务器
 * @details 该类用于创建TCP服务器，监听指定端口并接受客户端连接
 */
class TcpServer : public Object<TcpServer> {
public:
    using Self = TcpServer;

    /**
     * @brief 使用ip和端口构造TCP服务器
     * @param ip IP地址字符串
     * @param port 端口号
     * @param family 地址族，默认为AF_INET
     * @throws runtime_exception 如果创建套接字失败
     */
    TcpServer(const char* ip, u16 port, i32 family = AF_INET) :
            socket_(family, SOCK_STREAM) {
        socket_.bind(ip, port); // 绑定到指定IP和端口
        socket_.listen();       // 开始监听
    }

    /**
     * @brief 接受连接
     * @return 返回最新的客户端套接字
     * @throws runtime_exception 如果接受连接失败
     */
    Socket& accept() {
        clients_.append(socket_.accept()); // 接受客户端连接，保存客户端套接字
        return clients_.back(); // 返回最新的客户端套接字
    }

private:
    Socket socket_;             // TCP套接字
    util::Vec<Socket> clients_; // 已连接的客户端套接字
};

/**
 * @brief TCP客户端
 * @details 该类用于创建TCP客户端，连接到指定服务器地址和端口
 */
class TcpClient : public Object<TcpClient> {
public:
    using Self = TcpClient;

private:
    Socket socket_; // TCP套接字
};

} // namespace my::net

#endif // TCP_HPP