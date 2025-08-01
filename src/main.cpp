#include "Printer.hpp"
#include "Log.hpp"
#include "Exception.hpp"
#include "Socket.hpp"

int main() {
    system(("chcp " + std::to_string(CP_UTF8)).c_str()); // 控制台输出ASC颜色字符

    my::io::println(__cplusplus);

    // my::throw runtime_exception("some thing cause this runtime exception");
    my::Exception ex(my::ExceptionType::RuntimeException, "some thing cause this runtime exception");
    ex.log();
}