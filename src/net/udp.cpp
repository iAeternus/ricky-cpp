/**
 * @brief UDP - Rust风格API实现
 * @author Ricky
 * @date 2026/3/30
 * @version 1.0
 */
#include "net/udp.hpp"

namespace my::net {

UdpSocket UdpSocket::bind(str::StringView ip, u16 port) {
    return UdpSocket(ip, port);
}

UdpSocket UdpSocket::bind(u16 port) {
    return UdpSocket(str::StringView("0.0.0.0"), port);
}

UdpSocket::UdpSocket(str::StringView ip, u16 port) : handle_(nullptr, plat::net::close), local_ip_(""), local_port_(0) {
    plat::net::startup();
    auto* h = plat::net::create(plat::net::SocketFamily::Ipv4, plat::net::SocketType::Datagram);
    handle_.reset(h);
    plat::net::bind(handle_.get(), ip, port);
    plat::net::get_local_addr(handle_.get(), local_ip_, local_port_);
}

str::String<> UdpSocket::local_ip() const {
    return local_ip_;
}

u16 UdpSocket::local_port() const {
    return local_port_;
}

usize UdpSocket::send_to(str::StringView ip, u16 port, str::StringView data) {
    return plat::net::send_to(handle_.get(), data, data.len(), ip, port, 0);
}

UdpSocket::RecvResult UdpSocket::recv_from(usize max_size) {
    auto result = plat::net::recv_from(handle_.get(), max_size, 0);
    return {std::move(result.data), std::move(result.src_ip), result.src_port};
}

void UdpSocket::set_read_timeout(u32 timeout_ms) {
    plat::net::set_timeout_ms(handle_.get(), timeout_ms, true);
}

void UdpSocket::set_write_timeout(u32 timeout_ms) {
    plat::net::set_timeout_ms(handle_.get(), timeout_ms, false);
}

void UdpSocket::close() {
    handle_.reset();
    plat::net::cleanup();
}

} // namespace my::net
