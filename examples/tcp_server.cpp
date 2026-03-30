/**
 * @brief TCP Server Example
 * @author Ricky
 * @date 2026/3/30
 * @version 1.0
 */
#include "net/tcp.hpp"
#include "printer.hpp"

using namespace my;

int main() {
    io::println(str::String("=== TCP Server Demo ==="));

    auto listener = net::TcpListener::bind(str::StringView("127.0.0.1"), 8080);
    auto port = listener.local_port();
    io::print(str::String("Server listening on port: "), port);
    io::println(str::String("Waiting for client connection..."));

    auto client = listener.accept();
    if (client == nullptr) {
        io::println(str::String("Failed to accept connection"));
        listener.close();
        return 1;
    }

    io::println(str::String("Client connected!"));
    io::println(str::String("Receiving messages..."));

    int count = 0;
    while (count < 3) {
        auto data = client->read(1024);
        if (data.len() == 0) {
            io::println(str::String("Client disconnected"));
            break;
        }

        io::println(str::String("Message #"), ++count, str::String(": "), data.as_str());

        client->write(data.as_str());
    }

    client->close();
    listener.close();
    io::println(str::String("Server closed"));

    return 0;
}
