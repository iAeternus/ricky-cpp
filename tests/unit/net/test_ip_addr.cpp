#include "test_ip_addr.hpp"
#include "net/ip_addr.hpp"
#include "ricky_test.hpp"

namespace my::test::test_ip_addr {

void should_construct_ipv4() {
    auto ip = net::Ipv4Addr(192, 168, 1, 1);
    Assertions::assert_equals("192.168.1.1"_sv, ip.to_string().as_str());
}

void should_ipv4_any() {
    auto ip = net::Ipv4Addr::any();
    Assertions::assert_equals("0.0.0.0"_sv, ip.to_string().as_str());
}

void should_ipv4_loopback() {
    auto ip = net::Ipv4Addr::loopback();
    Assertions::assert_equals("127.0.0.1"_sv, ip.to_string().as_str());
}

void should_ipv4_broadcast() {
    auto ip = net::Ipv4Addr::broadcast();
    Assertions::assert_equals("255.255.255.255"_sv, ip.to_string().as_str());
}

void should_ipv4_to_u32() {
    auto ip = net::Ipv4Addr(192, 168, 1, 1);
    Assertions::assert_true(ip.to_u32() == 3232235777U);
}

void should_parse_ipv4_from_str() {
    auto ip = net::Ipv4Addr::from_str("192.168.1.100"_sv);
    Assertions::assert_equals("192.168.1.100"_sv, ip.to_string().as_str());
}

void should_construct_ipv6() {
    auto ip = net::Ipv6Addr::from_str("::1"_sv);
    Assertions::assert_equals("0:0:0:0:0:0:0:1"_sv, ip.to_string().as_str());
}

void should_ipv6_any() {
    auto ip = net::Ipv6Addr::any();
    Assertions::assert_equals("0:0:0:0:0:0:0:0"_sv, ip.to_string().as_str());
}

void should_ipv6_loopback() {
    auto ip = net::Ipv6Addr::loopback();
    Assertions::assert_equals("0:0:0:0:0:0:0:1"_sv, ip.to_string().as_str());
}

void should_parse_ipv6_from_str() {
    auto ip = net::Ipv6Addr::from_str("2001:db8::1"_sv);
    auto s = ip.to_string();
    Assertions::assert_true(s.len() > 0);
}

void should_ip_addr_is_ipv4() {
    auto ip = net::IpAddr::ipv4(192, 168, 1, 1);
    Assertions::assert_true(ip.is_ipv4());
    Assertions::assert_false(ip.is_ipv6());
}

void should_ip_addr_is_ipv6() {
    auto ip = net::IpAddr::ipv6(0, 0, 0, 0, 0, 0, 0, 1);
    Assertions::assert_false(ip.is_ipv4());
    Assertions::assert_true(ip.is_ipv6());
}

void should_ip_addr_from_str_ipv4() {
    auto ip = net::IpAddr::from_str("192.168.1.1"_sv);
    Assertions::assert_true(ip.is_ipv4());
}

void should_ip_addr_from_str_ipv6() {
    auto ip = net::IpAddr::from_str("::1"_sv);
    Assertions::assert_true(ip.is_ipv6());
}

GROUP_NAME("test_ip_addr")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct_ipv4),
    UNIT_TEST_ITEM(should_ipv4_any),
    UNIT_TEST_ITEM(should_ipv4_loopback),
    UNIT_TEST_ITEM(should_ipv4_broadcast),
    UNIT_TEST_ITEM(should_ipv4_to_u32),
    UNIT_TEST_ITEM(should_parse_ipv4_from_str),
    UNIT_TEST_ITEM(should_construct_ipv6),
    UNIT_TEST_ITEM(should_ipv6_any),
    UNIT_TEST_ITEM(should_ipv6_loopback),
    UNIT_TEST_ITEM(should_parse_ipv6_from_str),
    UNIT_TEST_ITEM(should_ip_addr_is_ipv4),
    UNIT_TEST_ITEM(should_ip_addr_is_ipv6),
    UNIT_TEST_ITEM(should_ip_addr_from_str_ipv4),
    UNIT_TEST_ITEM(should_ip_addr_from_str_ipv6))

} // namespace my::test::test_ip_addr
