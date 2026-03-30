/**
 * @brief TCP - Rust风格API实现
 * @author Ricky
 * @date 2026/3/30
 * @version 1.0
 */
#include "net/tcp.hpp"

namespace my::net {

TcpListener TcpListener::bind(str::StringView ip, u16 port) {
    return TcpListener(ip, port);
}

TcpListener TcpListener::bind(u16 port) {
    return TcpListener(str::StringView("0.0.0.0"), port);
}

TcpListener::TcpListener(str::StringView ip, u16 port) : handle_(nullptr, plat::net::close), local_ip_(), local_port_(0) {
    plat::net::startup();
    auto* h = plat::net::create(plat::net::SocketFamily::Ipv4, plat::net::SocketType::Stream);
    handle_.reset(h);
    plat::net::bind(handle_.get(), ip, port);
    plat::net::get_local_addr(handle_.get(), local_ip_, local_port_);
    plat::net::listen(handle_.get(), 128);
}

str::String<> TcpListener::local_ip() const {
    return local_ip_;
}

u16 TcpListener::local_port() const {
    return local_port_;
}

std::unique_ptr<TcpStream> TcpListener::accept() {
    auto* client_handle = plat::net::accept(handle_.get());
    return std::make_unique<TcpStream>(client_handle);
}

void TcpListener::close() {
    handle_.reset();
    plat::net::cleanup();
}

TcpStream TcpStream::connect(str::StringView ip, u16 port) {
    return TcpStream(ip, port);
}

TcpStream::TcpStream(str::StringView ip, u16 port) : handle_(nullptr, plat::net::close) {
    plat::net::startup();
    auto* h = plat::net::create(plat::net::SocketFamily::Ipv4, plat::net::SocketType::Stream);
    handle_.reset(h);
    plat::net::connect(handle_.get(), ip, port);
}

TcpStream::TcpStream(plat::net::SocketHandle* h) : handle_(h, plat::net::close) {}

str::String<> TcpStream::peer_ip() const {
    return peer_ip_;
}

u16 TcpStream::peer_port() const {
    return peer_port_;
}

usize TcpStream::write(str::StringView data) {
    return plat::net::send_bytes(handle_.get(), data, data.len(), 0);
}

str::String<> TcpStream::read(usize max_size) {
    return plat::net::recv_bytes(handle_.get(), max_size, 0);
}

void TcpStream::set_read_timeout(u32 timeout_ms) {
    plat::net::set_timeout_ms(handle_.get(), timeout_ms, true);
}

void TcpStream::set_write_timeout(u32 timeout_ms) {
    plat::net::set_timeout_ms(handle_.get(), timeout_ms, false);
}

void TcpStream::close() {
    handle_.reset();
}

bool TcpStream::is_open() const {
    return plat::net::is_valid(handle_.get());
}

} // namespace my::net
