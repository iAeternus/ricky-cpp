#include "net/ip_addr.hpp"

namespace my::net {

Ipv4Addr Ipv4Addr::from_str(str::StringView s) {
    u8 octets[4] = {0};
    usize idx = 0;
    for (usize i = 0; i < s.len() && idx < 4; ++i) {
        if (s.as_bytes()[i] == '.') {
            ++idx;
            continue;
        }
        if (s.as_bytes()[i] >= '0' && s.as_bytes()[i] <= '9') {
            octets[idx] = octets[idx] * 10 + (s.as_bytes()[i] - '0');
        }
    }
    return Ipv4Addr(octets[0], octets[1], octets[2], octets[3]);
}

Ipv4Addr Ipv4Addr::any() { return Ipv4Addr(0, 0, 0, 0); }
Ipv4Addr Ipv4Addr::loopback() { return Ipv4Addr(127, 0, 0, 1); }
Ipv4Addr Ipv4Addr::broadcast() { return Ipv4Addr(255, 255, 255, 255); }

str::String<> Ipv4Addr::to_string() const {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%u.%u.%u.%u", octets_[0], octets_[1], octets_[2], octets_[3]);
    return str::String<>(buf);
}

u32 Ipv4Addr::to_u32() const {
    return (static_cast<u32>(octets_[0]) << 24) | (static_cast<u32>(octets_[1]) << 16) | (static_cast<u32>(octets_[2]) << 8) | static_cast<u32>(octets_[3]);
}

Ipv6Addr Ipv6Addr::from_str(str::StringView s) {
    u16 segments[8] = {0};

    auto colon_pos_opt = s.find("::"_sv);
    if (colon_pos_opt.is_some()) {
        usize colon_pos = colon_pos_opt.unwrap();
        usize left_len = colon_pos;
        usize right_start = colon_pos + 2;
        usize right_len = s.len() - right_start;

        usize left_colons = 0;
        usize right_colons = 0;

        if (left_len > 0) {
            usize idx = 0;
            while (idx < left_len) {
                if (s.as_bytes()[idx] == ':') {
                    ++left_colons;
                }
                ++idx;
            }
        }

        if (right_len > 0) {
            usize idx = right_start;
            while (idx < s.len()) {
                if (s.as_bytes()[idx] == ':') {
                    ++right_colons;
                }
                ++idx;
            }
        }

        usize left_parts = (left_len > 0) ? (left_colons + 1) : 0;
        usize right_parts = (right_len > 0) ? (right_colons + 1) : 0;

        usize middle_parts = 8 - left_parts - right_parts;

        usize idx = 0;
        usize seg_idx = 0;

        while (idx < left_len && seg_idx < 8) {
            if (s.as_bytes()[idx] == ':') {
                ++idx;
                continue;
            }
            u16 val = 0;
            while (idx < left_len) {
                char c = static_cast<char>(s.as_bytes()[idx]);
                if (c >= '0' && c <= '9') {
                    val = val * 16 + (c - '0');
                } else if (c >= 'a' && c <= 'f') {
                    val = val * 16 + (c - 'a' + 10);
                } else if (c >= 'A' && c <= 'F') {
                    val = val * 16 + (c - 'A' + 10);
                } else {
                    break;
                }
                ++idx;
            }
            segments[seg_idx++] = val;
            if (idx < left_len && s.as_bytes()[idx] == ':') {
                ++idx;
            }
        }

        if (left_len == 0 && right_parts > 0) {
            seg_idx = 8 - right_parts;
        } else {
            seg_idx += middle_parts;
        }

        idx = right_start;
        while (idx < s.len() && seg_idx < 8) {
            if (s.as_bytes()[idx] == ':') {
                ++idx;
                continue;
            }
            u16 val = 0;
            while (idx < s.len()) {
                char c = static_cast<char>(s.as_bytes()[idx]);
                if (c >= '0' && c <= '9') {
                    val = val * 16 + (c - '0');
                } else if (c >= 'a' && c <= 'f') {
                    val = val * 16 + (c - 'a' + 10);
                } else if (c >= 'A' && c <= 'F') {
                    val = val * 16 + (c - 'A' + 10);
                } else {
                    break;
                }
                ++idx;
            }
            segments[seg_idx++] = val;
            if (idx < s.len() && s.as_bytes()[idx] == ':') {
                ++idx;
            }
        }
    } else {
        usize seg_idx = 0;
        usize i = 0;
        while (i < s.len() && seg_idx < 8) {
            if (s.as_bytes()[i] == ':') {
                ++i;
                continue;
            }
            u16 val = 0;
            while (i < s.len()) {
                char c = static_cast<char>(s.as_bytes()[i]);
                if (c >= '0' && c <= '9') {
                    val = val * 16 + (c - '0');
                } else if (c >= 'a' && c <= 'f') {
                    val = val * 16 + (c - 'a' + 10);
                } else if (c >= 'A' && c <= 'F') {
                    val = val * 16 + (c - 'A' + 10);
                } else {
                    break;
                }
                ++i;
            }
            segments[seg_idx++] = val;
            if (i < s.len() && s.as_bytes()[i] == ':') {
                ++i;
            }
        }
    }

    return Ipv6Addr(segments);
}

Ipv6Addr Ipv6Addr::any() { return Ipv6Addr::from_str(str::StringView("::")); }
Ipv6Addr Ipv6Addr::loopback() { return Ipv6Addr::from_str(str::StringView("::1")); }

Ipv6Addr::Ipv6Addr(u16 seg0, u16 seg1, u16 seg2, u16 seg3,
                   u16 seg4, u16 seg5, u16 seg6, u16 seg7) {
    segments_[0] = seg0;
    segments_[1] = seg1;
    segments_[2] = seg2;
    segments_[3] = seg3;
    segments_[4] = seg4;
    segments_[5] = seg5;
    segments_[6] = seg6;
    segments_[7] = seg7;
}

Ipv6Addr::Ipv6Addr(u16 (&segments)[8]) {
    for (int i = 0; i < 8; ++i) {
        segments_[i] = segments[i];
    }
}

str::String<> Ipv6Addr::to_string() const {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%x:%x:%x:%x:%x:%x:%x:%x",
                  segments_[0], segments_[1], segments_[2], segments_[3],
                  segments_[4], segments_[5], segments_[6], segments_[7]);
    return str::String<>(buf);
}

IpAddr IpAddr::from_str(str::StringView s) {
    if (s.len() > 0 && std::strchr(reinterpret_cast<const char*>(s.as_bytes()), ':') != nullptr) {
        return IpAddr(Ipv6Addr::from_str(s));
    }
    return IpAddr(Ipv4Addr::from_str(s));
}

IpAddr IpAddr::ipv4(u8 a, u8 b, u8 c, u8 d) {
    return IpAddr(Ipv4Addr(a, b, c, d));
}

IpAddr IpAddr::ipv6(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h) {
    return IpAddr(Ipv6Addr(a, b, c, d, e, f, g, h));
}

IpAddr::IpAddr(Ipv4Addr v4) : kind_(IpAddrKind::Ipv4), ipv4_(v4) {}
IpAddr::IpAddr(Ipv6Addr v6) : kind_(IpAddrKind::Ipv6), ipv6_(v6) {}

IpAddrKind IpAddr::kind() const { return kind_; }
bool IpAddr::is_ipv4() const { return kind_ == IpAddrKind::Ipv4; }
bool IpAddr::is_ipv6() const { return kind_ == IpAddrKind::Ipv6; }
const Ipv4Addr& IpAddr::as_ipv4() const { return ipv4_; }
const Ipv6Addr& IpAddr::as_ipv6() const { return ipv6_; }

str::String<> IpAddr::to_string() const {
    if (is_ipv4()) {
        return ipv4_.to_string();
    }
    return ipv6_.to_string();
}

} // namespace my::net
