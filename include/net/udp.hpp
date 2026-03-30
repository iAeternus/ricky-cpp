/**
 * @brief UDP - Rust风格API
 * @author Ricky
 * @date 2026/3/30
 * @version 1.0
 */
#ifndef NET_UDP_HPP
#define NET_UDP_HPP

#include "net.hpp"
#include "option.hpp"
#include "memory"

namespace my::net {

class UdpSocket {
public:
    static UdpSocket bind(str::StringView ip, u16 port);
    static UdpSocket bind(u16 port);

    UdpSocket(str::StringView ip, u16 port);

    [[nodiscard]] str::String<> local_ip() const;
    [[nodiscard]] u16 local_port() const;

    usize send_to(str::StringView ip, u16 port, str::StringView data);

    struct RecvResult {
        str::String<> data;
        str::String<> src_ip;
        u16 src_port{0};
    };

    RecvResult recv_from(usize max_size = 4096);

    void set_read_timeout(u32 timeout_ms);
    void set_write_timeout(u32 timeout_ms);

    void close();

private:
    std::unique_ptr<plat::net::SocketHandle, void (*)(plat::net::SocketHandle*)> handle_;
    str::String<> local_ip_;
    u16 local_port_{0};

    UdpSocket() : handle_(nullptr, plat::net::close) {}
};

} // namespace my::net

#endif // NET_UDP_HPP
