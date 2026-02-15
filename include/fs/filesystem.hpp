#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "my_config.hpp"

#if RICKY_WIN
#include "win/winlib.hpp"
#include "win/file_utils.hpp"
#include "win/file.hpp"

#elif RICKY_LINUX
#include "linux/linuxlib.hpp"
#include "linux/file_utils.hpp"
#include "linux/File.hpp"

#elif RICKY_MAC
#include "mac/maclib.hpp"
#include "mac/file_utils.hpp"
#include "mac/File.hpp"

#endif // OS

#endif // FILESYSTEM_HPP
