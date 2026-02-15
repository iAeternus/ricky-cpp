#include "unit/test_tcp.hpp"

#include "ricky_test.hpp"
#include "tcp.hpp"

#include <thread>

#include "test/test_registry.hpp"

namespace my::test::test_tcp {

void it_works() {
    const char* ip = "127.0.0.1";
    u16 port = 23456;
    CString msg = "hello tcp";

    // 启动服务器线程
    net::TcpServer server{ip, port};
    std::thread server_thread([&]() {
        auto client_sock = server.accept();
        auto recv_msg = client_sock->recv();
        Assertions::assertEquals(msg, recv_msg);
        client_sock->send(msg, msg.size());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待服务器启动

    // 客户端连接并收发数据
    net::TcpClient client{ip, port};
    client.send(msg.data(), msg.size());
    auto reply = client.recv();
    Assertions::assertEquals(reply, msg);

    server_thread.join(); // 等待服务器线程结束
}

void should_send_all() {
    const char* ip = "127.0.0.1";
    u16 port = 23457;
    CString msg = "broadcast tcp";

    // 启动服务器线程
    net::TcpServer server{ip, port};
    std::thread server_thread([&]() {
        // 接受两个客户端连接
        auto client1 = server.accept();
        auto client2 = server.accept();
        server.sendall(msg.data(), msg.size()); // 广播消息
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待服务器启动

    // 启动两个客户端并接收广播
    net::TcpClient client1{ip, port};
    net::TcpClient client2{ip, port};
    auto reply1 = client1.recv();
    auto reply2 = client2.recv();
    Assertions::assertEquals(reply1, msg);
    Assertions::assertEquals(reply2, msg);

    server_thread.join();
}

void test_tcp() {
    UnitTestGroup group{"test_tcp"};

    group.addTest("it_works", it_works);
    group.addTest("should_send_all", should_send_all);

    group.startAll();
}

GROUP_NAME("test_tcp")
REGISTER_UNIT_TESTS(
    UNIT_TEST_ITEM(it_works),
    UNIT_TEST_ITEM(should_send_all))
} // namespace my::test::test_tcp