/**
 * @brief Hello World!
 * @author Ricky
 * @date 2025/8/13
 * @version 1.0
 */
#include "printer.hpp"
#include "process.hpp"

int main() {
    my::plat::process::set_console_utf8();

    my::io::println("Hello, World!");
    my::io::println("C++ standard: ", __cplusplus);

    return 0;
}