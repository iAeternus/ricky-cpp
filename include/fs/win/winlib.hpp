#ifndef WIN_LIB_HPP
#define WIN_LIB_HPP

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#include <fileapi.h>
#include <WS2tcpip.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif // _MSC_VER

#endif // _WIN32

namespace my::fs::win {

} // namespace my::fs::win

#endif // WIN_LIB_HPP