#include "my_config.hpp"

#if RICKY_LINUX

#include "time.hpp"

namespace my::plat::time {

std::tm localtime(time_t t) {
    std::tm tm;
    localtime_r(&t, &tm);
    return tm;
}

} // namespace my::plat::time

#endif // RICKY_LINUX
