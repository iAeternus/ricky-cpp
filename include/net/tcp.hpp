/**
 * @brief TCP - Rust风格API
 * @author Ricky
 * @date 2026/3/30
 * @version 1.0
 */
#ifndef NET_TCP_HPP
#define NET_TCP_HPP

#include "net.hpp"
#include "vec.hpp"
#include "option.hpp"
#include "memory"

namespace my::net {

class TcpStream;

class TcpListener {
public:
    static TcpListener bind(str::StringView ip, u16 port);
    static TcpListener bind(u16 port);

    TcpListener(str::StringView ip, u16 port);

    [[nodiscard]] str::String<> local_ip() const;
    [[nodiscard]] u16 local_port() const;

    std::unique_ptr<TcpStream> accept();

    void close();

private:
    std::unique_ptr<plat::net::SocketHandle, void (*)(plat::net::SocketHandle*)> handle_;
    str::String<> local_ip_;
    u16 local_port_{0};

    TcpListener() : handle_(nullptr, plat::net::close) {}
};

class TcpStream {
public:
    static TcpStream connect(str::StringView ip, u16 port);

    TcpStream(str::StringView ip, u16 port);

    TcpStream(plat::net::SocketHandle* h);

    [[nodiscard]] str::String<> peer_ip() const;
    [[nodiscard]] u16 peer_port() const;

    usize write(str::StringView data);
    str::String<> read(usize max_size = 4096);

    void set_read_timeout(u32 timeout_ms);
    void set_write_timeout(u32 timeout_ms);

    void close();
    bool is_open() const;

private:
    std::unique_ptr<plat::net::SocketHandle, void (*)(plat::net::SocketHandle*)> handle_;
    str::String<> peer_ip_;
    u16 peer_port_{0};

    TcpStream() : handle_(nullptr, plat::net::close) {}
};

} // namespace my::net

#endif // NET_TCP_HPP
