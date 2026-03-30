#include "my_config.hpp"

#if RICKY_LINUX

#include "net.hpp"
#include "my_exception.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace my::plat::net {

struct SocketHandle {
    int fd{-1};
};

namespace {

int to_family(const SocketFamily family) {
    switch (family) {
    case SocketFamily::Ipv4: return AF_INET;
    case SocketFamily::Ipv6: return AF_INET6;
    default: return AF_INET;
    }
}

int to_type(const SocketType type) {
    switch (type) {
    case SocketType::Stream: return SOCK_STREAM;
    case SocketType::Datagram: return SOCK_DGRAM;
    default: return SOCK_STREAM;
    }
}

void fill_sockaddr(const str::StringView ip, const u16 port, sockaddr_storage& addr, socklen_t& len) {
    std::memset(&addr, 0, sizeof(addr));
    if (ip.len() == 0) {
        throw argument_exception("Invalid ip");
    }
    const auto ip_cstr = ip.into_cstr();

    if (std::strchr(ip_cstr.get(), ':') != nullptr) {
        auto* a6 = reinterpret_cast<sockaddr_in6*>(&addr);
        a6->sin6_family = AF_INET6;
        a6->sin6_port = htons(port);
        if (::inet_pton(AF_INET6, ip_cstr.get(), &a6->sin6_addr) != 1) {
            throw argument_exception("Invalid IPv6 address: {}", ip);
        }
        len = sizeof(sockaddr_in6);
        return;
    }

    auto* a4 = reinterpret_cast<sockaddr_in*>(&addr);
    a4->sin_family = AF_INET;
    a4->sin_port = htons(port);
    if (::inet_pton(AF_INET, ip_cstr.get(), &a4->sin_addr) != 1) {
        throw argument_exception("Invalid IPv4 address: {}", ip);
    }
    len = sizeof(sockaddr_in);
}

} // namespace

void startup() {}

void cleanup() {}

str::String<> last_error() {
    const auto err = errno;
    return str::String<>(std::strerror(err));
}

SocketHandle* create(const SocketFamily family, const SocketType type) {
    const int fd = ::socket(to_family(family), to_type(type), 0);
    if (fd < 0) {
        throw system_exception("Failed to create socket: {}", last_error());
    }
    auto* handle = new SocketHandle{};
    handle->fd = fd;
    return handle;
}

bool is_valid(SocketHandle* socket) {
    return socket != nullptr && socket->fd >= 0;
}

void close(SocketHandle* socket) {
    if (socket == nullptr) {
        return;
    }
    if (socket->fd >= 0) {
        ::close(socket->fd);
        socket->fd = -1;
    }
    delete socket;
}

void bind(SocketHandle* socket, const str::StringView ip, const u16 port) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    sockaddr_storage addr{};
    socklen_t len = 0;
    fill_sockaddr(ip, port, addr, len);
    if (::bind(socket->fd, reinterpret_cast<sockaddr*>(&addr), len) != 0) {
        throw system_exception("Bind failed: {}", last_error());
    }
}

void get_local_addr(SocketHandle* socket, str::String<>& ip, u16& port) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    sockaddr_storage addr{};
    socklen_t len = sizeof(addr);
    if (::getsockname(socket->fd, reinterpret_cast<sockaddr*>(&addr), &len) != 0) {
        throw system_exception("getsockname failed: {}", last_error());
    }
    if (addr.ss_family == AF_INET) {
        char ip_str[16];
        auto* a4 = reinterpret_cast<sockaddr_in*>(&addr);
        inet_ntop(AF_INET, &a4->sin_addr, ip_str, 16);
        ip = str::String<>(ip_str);
        port = ntohs(a4->sin_port);
    } else if (addr.ss_family == AF_INET6) {
        char ip_str[46];
        auto* a6 = reinterpret_cast<sockaddr_in6*>(&addr);
        inet_ntop(AF_INET6, &a6->sin6_addr, ip_str, 46);
        ip = str::String<>(ip_str);
        port = ntohs(a6->sin6_port);
    }
}

void listen(SocketHandle* socket, const i32 backlog) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    if (::listen(socket->fd, backlog) != 0) {
        throw system_exception("Listen failed: {}", last_error());
    }
}

SocketHandle* accept(SocketHandle* socket) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    const int fd = ::accept(socket->fd, nullptr, nullptr);
    if (fd < 0) {
        throw system_exception("Accept failed: {}", last_error());
    }
    auto* handle = new SocketHandle{};
    handle->fd = fd;
    return handle;
}

void connect(SocketHandle* socket, const str::StringView ip, const u16 port) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    sockaddr_storage addr{};
    socklen_t len = 0;
    fill_sockaddr(ip, port, addr, len);
    if (::connect(socket->fd, reinterpret_cast<sockaddr*>(&addr), len) != 0) {
        throw system_exception("Connect failed: {}", last_error());
    }
}

usize send_bytes(SocketHandle* socket, const str::StringView data, const usize size, const i32 flags) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    if (size > data.len()) {
        throw argument_exception("Invalid data pointer");
    }
    const auto* bytes = reinterpret_cast<const char*>(data.as_bytes());
    const auto sent = ::send(socket->fd, bytes, size, flags);
    if (sent < 0) {
        throw system_exception("Send failed: {}", last_error());
    }
    return static_cast<usize>(sent);
}

str::String<> recv_bytes(SocketHandle* socket, const usize size, const i32 flags) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    if (size == 0) {
        return str::String<>{};
    }
    std::vector<char> buffer(size);
    const auto received = ::recv(socket->fd, buffer.data(), size, flags);
    if (received < 0) {
        throw system_exception("Recv failed: {}", last_error());
    }
    if (received == 0) {
        return str::String<>{};
    }
    return str::String<>(buffer.data(), static_cast<usize>(received));
}

void set_timeout_ms(SocketHandle* socket, const u32 timeout_ms, const bool receive) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    timeval tv{};
    tv.tv_sec = static_cast<time_t>(timeout_ms / 1000);
    tv.tv_usec = static_cast<suseconds_t>((timeout_ms % 1000) * 1000);
    const int opt = receive ? SO_RCVTIMEO : SO_SNDTIMEO;
    if (::setsockopt(socket->fd, SOL_SOCKET, opt, &tv, sizeof(tv)) != 0) {
        throw system_exception("Set timeout failed: {}", last_error());
    }
}

void set_option(SocketHandle* socket, const i32 level, const i32 optname, const void* optval, const u32 optlen) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    if (::setsockopt(socket->fd, level, optname, reinterpret_cast<const char*>(optval), optlen) != 0) {
        throw system_exception("Set option failed: {}", last_error());
    }
}

usize send_to(SocketHandle* socket, const str::StringView data, const usize size, const str::StringView ip, const u16 port, const i32 flags) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    if (size > data.len()) {
        throw argument_exception("Send size exceeds data length");
    }
    sockaddr_storage addr{};
    socklen_t len = 0;
    fill_sockaddr(ip, port, addr, len);
    const auto* bytes = reinterpret_cast<const char*>(data.as_bytes());
    const auto sent = ::sendto(socket->fd, bytes, size, flags, reinterpret_cast<sockaddr*>(&addr), len);
    if (sent < 0) {
        throw system_exception("Sendto failed: {}", last_error());
    }
    return static_cast<usize>(sent);
}

UdpRecvResult recv_from(SocketHandle* socket, const usize size, const i32 flags) {
    if (!is_valid(socket)) {
        throw null_pointer_exception("Invalid socket");
    }
    UdpRecvResult result;
    if (size == 0) {
        return result;
    }
    std::vector<char> buffer(size);
    sockaddr_storage addr{};
    socklen_t len = sizeof(addr);
    const auto received = ::recvfrom(socket->fd, buffer.data(), size, flags, reinterpret_cast<sockaddr*>(&addr), &len);
    if (received < 0) {
        throw system_exception("Recvfrom failed: {}", last_error());
    }
    if (received == 0) {
        return result;
    }
    result.data = str::String<>(buffer.data(), static_cast<usize>(received));
    if (addr.ss_family == AF_INET) {
        auto* a4 = reinterpret_cast<sockaddr_in*>(&addr);
        char ip_str[16];
        inet_ntop(AF_INET, &a4->sin_addr, ip_str, 16);
        result.src_ip = str::String<>(ip_str);
        result.src_port = ntohs(a4->sin_port);
    } else if (addr.ss_family == AF_INET6) {
        char ip_str[46];
        auto* a6 = reinterpret_cast<sockaddr_in6*>(&addr);
        inet_ntop(AF_INET6, &a6->sin6_addr, ip_str, 46);
        result.src_ip = str::String<>(ip_str);
        result.src_port = ntohs(a6->sin6_port);
    }
    return result;
}

} // namespace my::plat::net

#endif // RICKY_LINUX
