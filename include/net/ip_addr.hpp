/**
 * @brief IP地址 - Rust风格API
 * @author Ricky
 * @date 2026/3/30
 * @version 1.0
 */
#ifndef NET_IP_ADDR_HPP
#define NET_IP_ADDR_HPP

#include "string.hpp"

namespace my::net {

enum class IpAddrKind : u8 {
    Ipv4,
    Ipv6
};

class Ipv4Addr {
public:
    static Ipv4Addr from_str(str::StringView s);
    static Ipv4Addr any();
    static Ipv4Addr loopback();
    static Ipv4Addr broadcast();

    constexpr Ipv4Addr(u8 a, u8 b, u8 c, u8 d) : octets_{a, b, c, d} {}

    [[nodiscard]] constexpr const u8* octets() const { return octets_; }
    [[nodiscard]] str::String<> to_string() const;
    [[nodiscard]] u32 to_u32() const;

private:
    u8 octets_[4];
};

class Ipv6Addr {
public:
    static Ipv6Addr from_str(str::StringView s);
    static Ipv6Addr any();
    static Ipv6Addr loopback();

    explicit Ipv6Addr(u16 seg0 = 0, u16 seg1 = 0, u16 seg2 = 0, u16 seg3 = 0,
                      u16 seg4 = 0, u16 seg5 = 0, u16 seg6 = 0, u16 seg7 = 0);
    explicit Ipv6Addr(u16 (&segments)[8]);

    [[nodiscard]] constexpr const u16* segments() const { return segments_; }
    [[nodiscard]] str::String<> to_string() const;

private:
    u16 segments_[8];
};

class IpAddr {
public:
    static IpAddr from_str(str::StringView s);
    static IpAddr ipv4(u8 a, u8 b, u8 c, u8 d);
    static IpAddr ipv6(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h);

    explicit IpAddr(Ipv4Addr v4);
    explicit IpAddr(Ipv6Addr v6);

    [[nodiscard]] IpAddrKind kind() const;
    [[nodiscard]] bool is_ipv4() const;
    [[nodiscard]] bool is_ipv6() const;
    [[nodiscard]] const Ipv4Addr& as_ipv4() const;
    [[nodiscard]] const Ipv6Addr& as_ipv6() const;

    [[nodiscard]] str::String<> to_string() const;

private:
    IpAddrKind kind_;
    union {
        Ipv4Addr ipv4_;
        Ipv6Addr ipv6_;
    };
};

} // namespace my::net

#endif // NET_IP_ADDR_HPP
