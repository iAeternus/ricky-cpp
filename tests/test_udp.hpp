#ifndef TEST_UDP_HPP
#define TEST_UDP_HPP

#include "ricky_test.hpp"
#include "udp.hpp"

#include <thread>

namespace my::test::test_udp {

auto it_works = []() {
    const char* ip = "127.0.0.1";
    u16 port = 12345;
    CString msg = "hello udp";

    // 启动服务器线程
    net::UdpServer server(ip, port);
    std::thread server_thread([&]() {
        auto [recv_msg, from] = server.recvfrom();
        Assertions::assertEquals(recv_msg, msg);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 等待服务器启动

    // 客户端发送数据
    net::UdpClient client(ip, port);
    client.sendto(msg.data(), msg.size());

    server_thread.join(); // 等待服务器线程结束
};

auto test_udp() {
    UnitTestGroup group{"test_udp"};

    group.addTest("it_works", it_works);

    group.startAll();
}

} // namespace my::test::test_udp

#endif // TEST_UDP_HPP