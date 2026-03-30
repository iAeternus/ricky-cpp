#ifndef TEST_NET_IP_ADDR_HPP
#define TEST_NET_IP_ADDR_HPP

namespace my::test::test_ip_addr {

void should_construct_ipv4();
void should_ipv4_any();
void should_ipv4_loopback();
void should_ipv4_broadcast();
void should_ipv4_to_u32();
void should_parse_ipv4_from_str();
void should_construct_ipv6();
void should_ipv6_any();
void should_ipv6_loopback();
void should_parse_ipv6_from_str();
void should_ip_addr_is_ipv4();
void should_ip_addr_is_ipv6();
void should_ip_addr_from_str_ipv4();
void should_ip_addr_from_str_ipv6();

} // namespace my::test::test_ip_addr

#endif
