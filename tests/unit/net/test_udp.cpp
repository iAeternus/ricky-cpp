#include "test_udp.hpp"
#include "net/udp.hpp"
#include "ricky_test.hpp"

namespace my::test::test_udp {

void should_construct_udp_socket() {
    auto sock = net::UdpSocket::bind(str::StringView("127.0.0.1"), 0);
    sock.close();
}

void should_construct_udp_socket_by_port() {
    auto sock = net::UdpSocket::bind(0);
    sock.close();
}

void should_udp_socket_close() {
    auto sock = net::UdpSocket::bind(str::StringView("127.0.0.1"), 0);
    sock.close();
}

void should_udp_send_to_and_recv_from() {
    auto server = net::UdpSocket::bind(str::StringView("127.0.0.1"), 0);
    auto server_port = server.local_port();

    auto client = net::UdpSocket::bind(str::StringView("127.0.0.1"), 0);

    auto sent = client.send_to(str::StringView("127.0.0.1"), server_port, str::StringView("Hello UDP"));
    Assertions::assertTrue(sent > 0);

    server.close();
    client.close();
}

GROUP_NAME("test_udp")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct_udp_socket),
    UNIT_TEST_ITEM(should_construct_udp_socket_by_port),
    UNIT_TEST_ITEM(should_udp_socket_close),
    UNIT_TEST_ITEM(should_udp_send_to_and_recv_from))

} // namespace my::test::test_udp
