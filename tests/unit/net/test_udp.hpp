#ifndef TEST_NET_UDP_HPP
#define TEST_NET_UDP_HPP

namespace my::test::test_udp {

void should_construct_udp_socket();
void should_construct_udp_socket_by_port();
void should_udp_socket_close();
void should_udp_send_to_and_recv_from();

} // namespace my::test::test_udp

#endif
