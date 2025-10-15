/**
 * @brief 测试日志框架
 * @author Ricky
 * @date 2025/10/11
 * @version 1.0
 */
#include "Log2.hpp"

#include <winnls.h>

void test_console_log() {
    my::log::console.info("hello {} {:d}", "world", 37);
}

int main() {
    system(("chcp " + std::to_string(CP_UTF8)).c_str()); // 控制台输出ASC颜色字符
    test_console_log();
}