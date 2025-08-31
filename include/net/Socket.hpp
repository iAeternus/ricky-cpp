/**
 * @brief 套接字
 * @author Ricky
 * @date 2025/7/13
 * @version 1.0
 */
#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "filesystem.hpp"
#include "Buffer.hpp"
#include "NoCopy.hpp"

#include <WinSock2.h>
#include <minwindef.h>

namespace my::net {

#if defined(RICKY_WIN)
/**
 * @brief Windows套接字初始化
 * @throws runtime_exception 如果WSAStartup失败
 */
fn win_startup() {
    static bool initialized = false;
    static std::mutex init_mutex;

    std::lock_guard<std::mutex> lock(init_mutex);
    if (initialized) return;
    initialized = true;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw runtime_exception("WSAStartup failed: {}", SRC_LOC, WSAGetLastError());
    }

    atexit([]() {
        WSACleanup();
    });
}

#define socket_startup() my::net::win_startup()

#elif defined(RICKY_LINUX)
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define socket_startup()
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

fn closesocket(i32 socket) {
    ::close(socket);
}

#endif // defined (RICKY_WIN)

/**
 * @brief 获取套接字错误信息
 * @return 错误信息字符串
 */
fn error_msg()->CString {
    CString error_msg{256};
#if _WIN32 || _WIN64
    int errorno = WSAGetLastError();
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorno,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error_msg.data(), 128, nullptr);
#else
    strerror(error_msg.data(), errno);
#endif // _WIN32 || _WIN64
    return error_msg;
}

/**
 * @brief 套接字地址
 */
class SockAddrIn : public Object<SockAddrIn> {
public:
    using Self = SockAddrIn;

    /**
     * @brief 默认构造函数
     * @details 初始化套接字地址结构为零
     */
    SockAddrIn() {
        memset(&addr_, 0, sizeof(sockaddr_in));
    }

    /**
     * @brief 构造函数
     * @param ip IP地址字符串
     * @param port 端口号
     * @param family 地址族，默认为AF_INET
     * @throws runtime_exception 如果IP地址转换失败
     */
    SockAddrIn(const char* ip, u16 port, i32 family = AF_INET) :
            SockAddrIn() {
        addr_.sin_family = family;
        addr_.sin_port = htons(port);

        if (ip == nullptr) {
            addr_.sin_addr.s_addr = INADDR_ANY; // 绑定到任意地址
        } else if (inet_pton(AF_INET, ip, &addr_.sin_addr) != 1) {
            throw runtime_exception(error_msg());
        }
    }

    /**
     * @brief 获取套接字地址结构
     * @return 指向sockaddr结构的指针
     */
    sockaddr* get_sockaddr() {
        return reinterpret_cast<sockaddr*>(&addr_);
    }

    /**
     * @brief 获取套接字地址结构（只读）
     * @return 指向sockaddr结构的常量指针
     */
    const sockaddr* get_sockaddr() const {
        return reinterpret_cast<const sockaddr*>(&addr_);
    }

    /**
     * @brief 获取套接字地址结构的长度
     * @return 套接字地址结构的大小
     */
    u32 get_socklen() const {
        return sizeof(sockaddr_in);
    }

    /**
     * @brief 获取IP地址字符串
     * @param family 地址族，默认为AF_INET
     * @return IP地址字符串
     * @throws runtime_exception 如果IP地址转换失败
     */
    CString get_ip(int family = AF_INET) const {
        CString ip{16};
        if (inet_ntop(family, &addr_.sin_addr, ip.data(), 16) == nullptr) {
            throw runtime_exception(error_msg());
        }
        return ip;
    }

    /**
     * @brief 获取端口号
     * @return 端口号
     */
    u16 get_port() const {
        return ntohs(addr_.sin_port);
    }

    [[nodiscard]] CString __str__() const {
        return std::format("{}:{}", get_ip(), get_port());
    }

private:
    sockaddr_in addr_; // 套接字地址结构
};

/**
 * @brief 套接字类
 * @details 封装了套接字的基本操作
 */
class Socket : public Object<Socket>, public NoCopyMove {
public:
    using Self = Socket;

    /**
     * @brief 使用现有套接字构造
     * @param socket 已存在的套接字描述符
     */
    Socket(u64 socket) :
            socket_(socket) {
        if (socket_ == INVALID_SOCKET) {
            throw runtime_exception("Invalid socket");
        }
    }

    /**
     * @brief 使用地址族和类型构造
     * @param family 地址族，默认为AF_INET
     * @param type 套接字类型，默认为SOCK_STREAM
     * @throws runtime_exception 如果创建套接字失败
     * @details 根据类型选择默认协议：
     * - SOCK_STREAM: IPPROTO_TCP
     * - SOCK_DGRAM: IPPROTO_UDP
     * - SOCK_RAW: IPPROTO_RAW
     * - SOCK_RDM: 不支持
     * - SOCK_SEQPACKET: 不支持
     */
    Socket(i32 family, i32 type) {
        socket_startup();

        i32 protocol = 0; // 默认协议
        switch (type) {
        case SOCK_STREAM:
            protocol = IPPROTO_TCP;
            break;
        case SOCK_DGRAM:
            protocol = IPPROTO_UDP;
            break;
        case SOCK_RAW:
            protocol = IPPROTO_RAW;
            break;
        case SOCK_RDM:
        case SOCK_SEQPACKET:
            throw runtime_exception("Unsupported socket type");
        default:
            protocol = 0; // 让系统选择默认协议
        }

        socket_ = socket(family, type, protocol);
        if (socket_ == INVALID_SOCKET) {
            throw runtime_exception(error_msg());
        }

        // 设置地址重用选项
        i32 optval = 1;
        setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&optval), sizeof(optval));
    }

    /**
     * @brief 移动构造函数
     * @param other 另一个Socket对象
     */
    Socket(Self&& other) noexcept :
            socket_(other.socket_) {
        other.socket_ = INVALID_SOCKET; // 避免析构时关闭套接字
    }

    /**
     * @brief 析构函数
     * @details 关闭套接字
     */
    ~Socket() {
        close(); // 析构时关闭套接字
    }

    /**
     * @brief 移动赋值
     */
    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;
        close();
        socket_ = other.socket_;
        other.socket_ = INVALID_SOCKET; // 避免析构时关闭套接字
        return *this;
    }

    /**
     * @brief 获取套接字描述符
     */
    u64 descriptor() const {
        return socket_;
    }

    /**
     * @brief 获取本地地址信息
     * @return 包含本地IP和端口的SockAddrIn对象
     * @throws runtime_exception 如果获取地址信息失败
     */
    SockAddrIn get_local_address() const {
        SockAddrIn addr;
        socklen_t len = addr.get_socklen();
        if (getsockname(socket_, addr.get_sockaddr(), &len) == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        }
        return addr;
    }

    /**
     * @brief 获取远程地址信息
     * @return 包含远程IP和端口的SockAddrIn对象
     * @throws runtime_exception 如果获取地址信息失败
     */
    SockAddrIn get_remote_address() const {
        SockAddrIn addr;
        socklen_t len = addr.get_socklen();
        if (getpeername(socket_, addr.get_sockaddr(), &len) == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        }
        return addr;
    }

    /**
     * @brief 获取本地IP地址
     * @return 本地IP地址字符串
     * @throws runtime_exception 如果获取地址信息失败
     */
    CString get_local_ip() const {
        return get_local_address().get_ip();
    }

    /**
     * @brief 获取本地端口号
     * @return 本地端口号
     * @throws runtime_exception 如果获取地址信息失败
     */
    u16 get_local_port() const {
        return get_local_address().get_port();
    }

    /**
     * @brief 获取远程IP地址
     * @return 远程IP地址字符串
     * @throws runtime_exception 如果获取地址信息失败
     */
    CString get_remote_ip() const {
        return get_remote_address().get_ip();
    }

    /**
     * @brief 获取远程端口号
     * @return 远程端口号
     * @throws runtime_exception 如果获取地址信息失败
     */
    u16 get_remote_port() const {
        return get_remote_address().get_port();
    }

    /**
     * @brief 连接到指定地址和端口
     * @param ip IP地址字符串
     * @param port 端口号
     * @throws runtime_exception 如果连接失败
     */
    void bind(const char* ip, u16 port) const {
        SockAddrIn addr(ip, port);
        if (::bind(socket_, addr.get_sockaddr(), addr.get_socklen()) == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        }
    }

    /**
     * @brief 监听端口
     * @param backlog 等待连接的队列长度，默认为SOMAXCONN
     * @throws runtime_exception 如果监听失败
     */
    void listen(i32 backlog = SOMAXCONN) const {
        if (::listen(socket_, backlog) == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        }
    }

    /**
     * @brief 接受连接
     * @return 返回一个新的Socket对象，表示已连接的客户端套接字
     * @throws runtime_exception 如果接受连接失败
     */
    Self accept() const {
        SockAddrIn client_addr;
        socklen_t addr_len = client_addr.get_socklen();

        auto client_socket = ::accept(socket_, client_addr.get_sockaddr(), &addr_len);
        if (client_socket == INVALID_SOCKET) {
            throw runtime_exception(error_msg());
        }
        return Self(client_socket);
    }

    /**
     * @brief 连接到指定地址和端口
     * @details 该方法会阻塞直到连接成功或失败
     * @param ip IP地址字符串
     * @param port 端口号
     * @throws runtime_exception 如果连接失败
     */
    void connect(const char* ip, u16 port) const {
        SockAddrIn addr(ip, port);
        if (::connect(socket_, addr.get_sockaddr(), addr.get_socklen()) == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        }
    }

    /**
     * @brief 发送字节数据
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param flags 发送标志，默认为0
     * @throws runtime_exception 如果发送失败
     */
    void send_bytes(const char* data, usize size, i32 flags = 0) const {
        const char* ptr = data;
        while (size > 0) {
            auto num_send = ::send(socket_, ptr, size, flags);
            if (num_send == SOCKET_ERROR) {
                throw runtime_exception(error_msg());
            } else if (num_send == 0) {
                continue;
            }
            ptr += num_send;
            size -= num_send;
        }
    }

    /**
     * @brief 接收字节数据
     * @param size 要接收的数据大小
     * @param flags 接收标志，默认为0
     * @return 返回接收到的数据
     * @throws runtime_exception 如果接收失败
     */
    CString recv_bytes(i32 size, i32 flags = 0) const {
        CString data{static_cast<usize>(size)};
        auto* ptr = data.data();
        while (size > 0) {
            auto recvd = ::recv(socket_, ptr, size, flags);
            if (recvd == SOCKET_ERROR) {
                throw runtime_exception(error_msg());
            }
            ptr += recvd;
            size -= recvd;
        }
        return data;
    }

    /**
     * @brief 发送数据，需要用recv接收
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param flags 发送标志，默认为0
     * @throws runtime_exception 如果发送失败
     */
    void send(const char* data, usize size, i32 flags = 0) const {
        util::Buffer<char> head_data{size + 4};
        auto head_size = htonl(size);
        head_data.append_bytes(&head_size, 4);
        head_data.append_bytes(data, size);

        send_bytes(head_data.data(), size + 4, flags);
    }

    /**
     * @brief 接收数据
     * @param flags 接收标志，默认为0
     * @return 返回接收到的数据
     * @throws runtime_exception 如果接收失败
     */
    CString recv(i32 flags = 0) const {
        i32 head_size = 0;
        auto recvd = ::recv(socket_, reinterpret_cast<char*>(&head_size), 4, flags);
        if (recvd == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        } else if (recvd == 0) {
            return CString{}; // 连接已关闭
        }

        return recv_bytes(ntohl(head_size), flags);
    }

    /**
     * @brief 发送数据到指定地址，需要用recvfrom接收
     * @param data 要发送的数据指针
     * @param size 数据大小
     * @param to 目标地址
     * @param flags 发送标志，默认为0
     * @throws runtime_exception 如果发送失败
     */
    void sendto(const char* data, i32 size, const SockAddrIn& to, i32 flags = 0) const {
        auto num_send = ::sendto(socket_, data, size, flags, to.get_sockaddr(), to.get_socklen());
        if (num_send == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        } else if (num_send != size) {
            throw runtime_exception(std::format("failed to send all data.{}/{}", num_send, size));
        }
    }

    /**
     * @brief 接收数据
     * @param flags 接收标志，默认为0
     * @return 返回接收到的数据和发送方地址
     * @throws runtime_exception 如果接收失败
     */
    Pair<CString, SockAddrIn> recvfrom(i32 flags = 0) const {
        SockAddrIn from{};
        CString data{1024}; // 初始大小为1024字节
        i32 addr_len = from.get_socklen();
        if (::recvfrom(socket_, data.data(), data.size(), flags, from.get_sockaddr(), &addr_len) == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        }
        return {data, from};
    }

    /**
     * @brief 设置套接字缓冲区大小
     * @param size 缓冲区大小
     * @param mode 缓冲区模式，"r"表示接收缓冲区，"w"表示发送缓冲区
     * @throws runtime_exception 如果设置缓冲区大小失败
     */
    void setbuffer(i32 size, CString mode) const {
        if (mode == "r") {
            ::setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&size), sizeof(size));
        } else if (mode == "w") {
            ::setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&size), sizeof(size));
        } else {
            throw runtime_exception(std::format("invalid buffer mode {}. Should be 'r' or 'w'.", mode));
        }
    }

    /**
     * @brief 设置套接字选项
     * @param level 选项级别
     * @param optname 选项名
     * @param optval 选项值
     * @param optlen 选项值长度
     * @throws runtime_exception 如果设置失败
     */
    void set_option(i32 level, i32 optname, const void* optval, socklen_t optlen) const {
        if (setsockopt(socket_, level, optname, reinterpret_cast<const char*>(optval), optlen) == SOCKET_ERROR) {
            throw runtime_exception(error_msg());
        }
    }

    /**
     * @brief 设置套接字超时
     * @param timeout_ms 超时时间(毫秒)
     * @param receive 是否为接收超时(true)或发送超时(false)
     * @throws runtime_exception 如果设置失败
     */
    void set_timeout(u32 timeout_ms, bool receive = true) const {
#if defined(RICKY_WIN)
        DWORD timeout = timeout_ms;
        set_option(SOL_SOCKET, receive ? SO_RCVTIMEO : SO_SNDTIMEO, &timeout, sizeof(timeout));
#else
        struct timeval tv {
            .tv_sec = static_cast<time_t>(timeout_ms / 1000),
            .tv_usec = static_cast<suseconds_t>((timeout_ms % 1000) * 1000)
        };
        set_option(SOL_SOCKET, receive ? SO_RCVTIMEO : SO_SNDTIMEO, &tv, sizeof(tv));
#endif
    }

    /**
     * @brief 关闭套接字
     * @details 关闭套接字并将其设置为无效状态
     */
    void close() {
        if (socket_ != INVALID_SOCKET) {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
    }

    /**
     * @brief 检查套接字是否有效
     */
    bool is_valid() const {
        return socket_ != INVALID_SOCKET;
    }

    [[nodiscard]] CString __str__() const {
        return CString{std::format("Socket({})", socket_)};
    }

private:
    u64 socket_ = INVALID_SOCKET; // 套接字描述符
};

} // namespace my::net

#endif // SOCKET_HPP