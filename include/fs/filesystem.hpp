#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#if defined(_WIN32) || defined(_WIN64)
#define RICKY_WIN
#include "win/winlib.hpp"
#include "win/file_utils.hpp"
#include "win/File.hpp"

#elif defined(__linux__) || defined(__unix__)
#define RICKY_LINUX
#include "linux/linuxlib.hpp"
#include "linux/file_utils.hpp"
#include "linux/File.hpp"

#elif defined(__APPLE__)
#define RICKY_MAC
#include "mac/maclib.hpp"
#include "mac/file_utils.hpp"
#include "mac/File.hpp"

#endif // OS

#endif // FILESYSTEM_HPP