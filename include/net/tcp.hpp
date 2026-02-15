/**
 * @brief TCP服务器、客户端
 * @author Ricky
 * @date 2025/7/13
 * @version 1.0
 */
#ifndef TCP_HPP
#define TCP_HPP

#include "socket.hpp"
#include "vec.hpp"

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
        // 设置优雅关闭选项
        linger lin{.l_onoff = 1, .l_linger = 5};
        socket_.set_option(SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));

        socket_.bind(ip, port);
        socket_.listen();
    }

    /**
     * @brief 获取本地IP地址
     * @return 本地IP地址字符串
     * @throws runtime_exception 如果获取地址信息失败
     */
    CString get_local_ip() const {
        return socket_.get_local_ip();
    }

    /**
     * @brief 获取本地端口号
     * @return 本地端口号
     * @throws runtime_exception 如果获取地址信息失败
     */
    u16 get_local_port() const {
        return socket_.get_local_port();
    }

    /**
     * @brief 获取远程IP地址
     * @return 远程IP地址字符串
     * @throws runtime_exception 如果获取地址信息失败
     */
    CString get_remote_ip() const {
        return socket_.get_remote_ip();
    }

    /**
     * @brief 获取远程端口号
     * @return 远程端口号
     * @throws runtime_exception 如果获取地址信息失败
     */
    u16 get_remote_port() const {
        return socket_.get_remote_port();
    }

    /**
     * @brief 接受连接
     * @return 返回最新的客户端套接字
     * @throws runtime_exception 如果接受连接失败
     */
    std::shared_ptr<Socket> accept() {
        auto client = std::make_shared<Socket>(socket_.accept());
        clients_.push(client);
        return client;
    }

    /**
     * @brief 发送数据到指定客户端
     * @param index 客户端索引
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param flags 发送标志，默认为0
     * @throws runtime_exception 如果发送失败
     */
    void send(i32 index, const char* data, usize size, i32 flags = 0) const {
        cliend(index).send(data, size, flags);
    }

    /**
     * @brief 广播数据到所有已连接的客户端
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param flags 发送标志，默认为0
     */
    void sendall(const char* data, usize size, i32 flags = 0) const {
        for (const auto& client : clients_) {
            client.lock()->send(data, size, flags);
        }
    }

    /**
     * @brief 接收数据
     * @param index 客户端索引
     * @param flags 接收标志，默认为0
     * @return 返回接收到的数据
     * @throws runtime_exception 如果接收失败
     */
    CString recv(i32 index, i32 flags = 0) const {
        return cliend(index).recv(flags); // 接收数据，假设每次接收1024字节
    }

    /**
     * @brief 根据客户端索引获取客户端套接字
     * @param index 客户端索引
     */
    const Socket& cliend(i32 index) const {
        return *clients_[index].lock();
    }

    /**
     * @brief 关闭服务器
     */
    void close() {
        for (auto& client : clients_) {
            if (auto ptr = client.lock()) {
                ptr->close();
            }
        }
        clients_.clear();
        socket_.close();
    }

private:
    Socket socket_;                            // 服务器套接字
    util::Vec<std::weak_ptr<Socket>> clients_; // 客户端连接
};

/**
 * @brief TCP客户端
 * @details 该类用于创建TCP客户端，连接到指定服务器地址和端口
 */
class TcpClient : public Object<TcpClient> {
public:
    using Self = TcpClient;

    /**
     * @brief 使用地址族构造TCP客户端
     * @param family 地址族，默认为AF_INET
     * @throws runtime_exception 如果创建套接字失败
     */
    TcpClient(const char* ip, u16 port, i32 family = AF_INET) :
            socket_(family, SOCK_STREAM) {
        socket_.connect(ip, port);
    }

    /**
     * @brief 发送数据到服务器
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param flags 发送标志，默认为0
     */
    void send(const char* data, usize size, i32 flags = 0) const {
        socket_.send(data, size, flags);
    }

    /**
     * @brief 接收数据
     * @param flags 接收标志，默认为0
     * @return 返回接收到的数据
     * @throws runtime_exception 如果接收失败
     */
    CString recv(i32 flags = 0) const {
        return socket_.recv(flags);
    }

private:
    Socket socket_; // TCP套接字
};

} // namespace my::net

#endif // TCP_HPP