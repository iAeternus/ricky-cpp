#include "my_config.hpp"

#if RICKY_WIN

#include "process.hpp"

#include <Windows.h>

namespace my::plat::process {

u32 pid() {
    return static_cast<u32>(::GetCurrentProcessId());
}

void set_console_utf8() {
    ::SetConsoleOutputCP(CP_UTF8);
    ::SetConsoleCP(CP_UTF8);
}

} // namespace my::plat::process

#endif // RICKY_WIN
