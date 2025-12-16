/**
 * @brief 测试日志框架
 * @author Ricky
 * @date 2025/10/11
 * @version 1.0
 */
#include "log.hpp"

#include <winnls.h>

void test_console_log() {
    my::log::set_consolelog_level(my::log::LogLevel::Trace);
    my::log::console.trace("This is a {} log.", "trace");
    my::log::console.debug("This is a {} log.", "debug");
    my::log::console.info("This is a {} log.", "info");
    my::log::console.warn("This is a {} log.", "warn");
    my::log::console.error("This is a {} log.", "error");
    my::log::console.fatal("This is a {} log.", "fatal");
}

int main() {
    system(("chcp " + std::to_string(CP_UTF8)).c_str()); // 控制台输出ASC颜色字符
    test_console_log();
    return 0;
}