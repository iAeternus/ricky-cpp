#ifndef PLAT_TIME_HPP
#define PLAT_TIME_HPP

#include "my_types.hpp"
#include <ctime>

namespace my::plat::time {

std::tm localtime(time_t t);

} // namespace my::plat::time

#endif // PLAT_TIME_HPP
