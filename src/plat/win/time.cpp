#include "sys.hpp"

#if RICKY_WIN

#include "time.hpp"

namespace my::plat::time {

std::tm localtime(time_t t) {
    std::tm tm;
    localtime_s(&tm, &t);
    return tm;
}

} // namespace my::plat::time

#endif // RICKY_WIN
