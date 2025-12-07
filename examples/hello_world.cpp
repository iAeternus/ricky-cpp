/**
 * @brief Hello World!
 * @author Ricky
 * @date 2025/8/13
 * @version 1.0
 */
#include "printer.hpp"

#include <winnls.h>

int main() {
    system(("chcp " + std::to_string(CP_UTF8)).c_str()); // 控制台输出ASC颜色字符

    my::io::println("Hello, World!");
    my::io::print("C++ standard: ", __cplusplus);
}