/**
 * @brief win文件工具
 * @author Ricky
 * @date 2025/1/1
 * @version 1.0
 */
#ifndef WIN_FILE_UTILS_HPP
#define WIN_FILE_UTILS_HPP

#include "DynArray.hpp"
#include "Exception.hpp"

#include <fileapi.h>
#include <handleapi.h>
#include <winerror.h>
#include <wingdi.h>

namespace my::fs::win {

static constexpr char PATH_SEP = '\\';

/**
 * @brief 判断文件或文件夹是否存在
 * @return true=存在 false=不存在
 */
fn exists(const char* path)->bool {
    auto attributes = GetFileAttributesA(path);
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

/**
 * @brief 判断是否为文件
 * @return true=是 false=否
 */
fn isfile(const char* path)->bool {
    auto attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

/**
 * @brief 判断是否为文件夹
 * @return true=是 false=否
 */
fn isdir(const char* path)->bool {
    auto attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

/**
 * @brief 创建一个文件夹
 * @return void
 */
fn mkdir(const char* path, bool exist_ok = false) {
    auto state = CreateDirectoryA(path, nullptr);

    switch (state) {
    case ERROR_ALREADY_EXISTS:
        if (!exist_ok) {
            throw runtime_exception("directory already exists: {}", SRC_LOC, path);
        }
        break;
    case ERROR_PATH_NOT_FOUND:
        throw not_found_exception("path not found: {}", SRC_LOC, path);
        break;
    }
}

/**
 * @brief 删除一个文件或文件夹
 * @return void
 */
fn remove(const char* path) {
    if (!exists(path)) {
        throw not_found_exception("file or directory not found in {}", SRC_LOC, path);
    }

    if (isfile(path) && !DeleteFileA(path)) {
        throw system_exception("failed to remove file");
    } else if (isdir(path) && !RemoveDirectoryA(path)) {
        throw system_exception("failed to remove directory");
    }
}

/**
 * @brief 拼接两个路径，若第一个路径结尾没有分隔符，会添加一个分隔符
 * @return 拼接后的路径
 */
fn join(const char* path1, const char* path2)->CString {
    auto length1 = std::strlen(path1);
    auto length2 = std::strlen(path2);
    auto length = length1 + length2 + (path1[length1 - 1] != '\\' && path1[length1 - 1] != '/');

    CString result{length};
    std::memcpy(result.data(), path1, length1);

    if (length1 + length2 != length) {
        result[length1] = PATH_SEP;
    }

    std::memcpy(result.data() + length - length2, path2, length2);
    return result;
}

/**
 * @brief 列出文件夹中的文件名
 * @return 文件名集合
 */
fn listdir(const char* path)->util::DynArray<CString> {
    WIN32_FIND_DATAA find_data;
    auto handle = FindFirstFileA(join(path, "*"), &find_data);

    if (handle == INVALID_HANDLE_VALUE) {
        throw system_exception("failed to list directory: {}", SRC_LOC, path);
    }

    util::DynArray<CString> results;
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        results.append(find_data.cFileName);
    } while (FindNextFileA(handle, &find_data));

    FindClose(handle);
    return results;
}

} // namespace my::fs::win

#endif // WIN_FILE_UTILS_HPP