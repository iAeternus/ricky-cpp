#include "test_tcp.hpp"
#include "net/tcp.hpp"
#include "ricky_test.hpp"

namespace my::test::test_tcp {

void should_construct_tcp_listener() {
    auto listener = net::TcpListener::bind(str::StringView("127.0.0.1"), 0);
    listener.close();
}

void should_construct_tcp_listener_by_port() {
    auto listener = net::TcpListener::bind(0);
    listener.close();
}

void should_tcp_listener_close() {
    auto listener = net::TcpListener::bind(str::StringView("127.0.0.1"), 0);
    listener.close();
}

void should_tcp_listener_accept() {
    auto listener = net::TcpListener::bind(str::StringView("127.0.0.1"), 0);
    auto port = listener.local_port();

    auto client = net::TcpStream::connect(str::StringView("127.0.0.1"), port);
    auto server = listener.accept();
    Assertions::assertTrue(server != nullptr);

    client.close();
    server->close();
    listener.close();
}

GROUP_NAME("test_tcp")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(should_construct_tcp_listener),
    UNIT_TEST_ITEM(should_construct_tcp_listener_by_port),
    UNIT_TEST_ITEM(should_tcp_listener_close),
    UNIT_TEST_ITEM(should_tcp_listener_accept))

} // namespace my::test::test_tcp
