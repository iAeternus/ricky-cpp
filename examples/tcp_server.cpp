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
    io::println("=== TCP Server Demo ===");

    auto listener = net::TcpListener::bind("127.0.0.1"_sv, 8080);
    auto port = listener.local_port();
    io::print("Server listening on port: ", port);
    io::println("Waiting for client connection...");

    auto client = listener.accept();
    if (client == nullptr) {
        io::println("Failed to accept connection");
        listener.close();
        return 1;
    }

    io::println("Client connected!");
    io::println("Receiving messages...");

    int count = 0;
    while (count < 3) {
        auto data = client->read(1024);
        if (data.len() == 0) {
            io::println("Client disconnected");
            break;
        }

        io::println("Message #", ++count, ": ", data.as_str());

        client->write(data.as_str());
    }

    client->close();
    listener.close();
    io::println("Server closed");

    return 0;
}
