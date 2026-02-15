#ifndef WIN_LIB_HPP
#define WIN_LIB_HPP

#include "my_config.hpp"

#if RICKY_WIN
#include <WinSock2.h>
#include <Windows.h>
#include <fileapi.h>
#include <WS2tcpip.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif // _MSC_VER

#endif // RICKY_WIN

namespace my::fs::win {

} // namespace my::fs::win

#endif // WIN_LIB_HPP
