#ifndef LINUX_LIB_HPP
#define LINUX_LIB_HPP

#if defined(__linux__) || defined(__unix__)
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // __linux__ || __unix__

namespace my::fs::linux {

} // namespace my::fs::linux

#endif // LINUX_LIB_HPP