#ifndef LINUX_LIB_HPP
#define LINUX_LIB_HPP

#include "my_config.hpp"

#if RICKY_LINUX
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // RICKY_LINUX

namespace my::fs::linux {

} // namespace my::fs::linux

#endif // LINUX_LIB_HPP
