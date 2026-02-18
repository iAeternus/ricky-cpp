#include "my_config.hpp"

#if RICKY_LINUX

#include "process.hpp"

#include <unistd.h>

namespace my::plat::process {

u32 pid() {
    return static_cast<u32>(::getpid());
}

void set_console_utf8() {}

} // namespace my::plat::process

#endif // RICKY_LINUX
