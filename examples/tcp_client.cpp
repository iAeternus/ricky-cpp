/**
 * @brief TCP Client Example
 * @author Ricky
 * @date 2026/3/30
 * @version 1.0
 */
#include "net/tcp.hpp"
#include "printer.hpp"

using namespace my;

int main() {
    io::println(str::String("=== TCP Client Demo ==="));

    auto client = net::TcpStream::connect(str::StringView("127.0.0.1"), 8080);
    io::println(str::String("Connected to server!"));

    const char* msg1 = "MSG1";
    const char* msg2 = "MSG2";
    const char* msg3 = "MSG3";

    io::print(str::String("Sending: "), str::String(msg1));
    client.write(str::StringView(msg1));

    auto resp = client.read(1024);
    io::println(str::String("Response length: "), resp.len());

    io::print(str::String("Sending: "), str::String(msg2));
    client.write(str::StringView(msg2));

    resp = client.read(1024);
    io::println(str::String("Response length: "), resp.len());

    io::print(str::String("Sending: "), str::String(msg3));
    client.write(str::StringView(msg3));

    resp = client.read(1024);
    io::println(str::String("Response length: "), resp.len());

    client.close();
    io::println(str::String("Connection closed"));

    return 0;
}
