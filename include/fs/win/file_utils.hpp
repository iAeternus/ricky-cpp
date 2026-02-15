/**
 * @brief win文件工具
 * @author Ricky
 * @date 2025/1/1
 * @version 1.0
 */
#ifndef WIN_FILE_UTILS_HPP
#define WIN_FILE_UTILS_HPP

#include "vec.hpp"

#include <fileapi.h>
#include <handleapi.h>
#include <winerror.h>

namespace my::fs::win {

static constexpr char PATH_SEP = '\\';

/**
 * @brief 判断文件或文件夹是否存在
 * @return true=存在 false=不存在
 */
auto exists(const char* path) -> bool {
    auto attributes = GetFileAttributesA(path);
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

/**
 * @brief 判断是否为文件
 * @return true=是 false=否
 */
auto isfile(const char* path) -> bool {
    auto attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

/**
 * @brief 判断是否为文件夹
 * @return true=是 false=否
 */
auto isdir(const char* path) -> bool {
    auto attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

/**
 * @brief 创建一个文件夹
 * @return void
 */
auto mkdir(const char* path, bool exist_ok = false) {
    auto state = CreateDirectoryA(path, nullptr); // TODO 由于win api的问题，这里返回值只有0或1，不知道为什么

    switch (state) {
    case ERROR_ALREADY_EXISTS:
        if (!exist_ok) {
            throw runtime_exception("Directory already exists: {}", path);
        }
        break;
    case ERROR_PATH_NOT_FOUND:
        throw not_found_exception("Path not found: {}", path);
        break;
    }
}

/**
 * @brief 删除一个文件或文件夹
 * @return void
 */
auto remove(const char* path) {
    if (!exists(path)) {
        throw not_found_exception("File or directory not found in {}", path);
    }

    if (isfile(path) && !DeleteFileA(path)) {
        throw system_exception("Failed to remove file");
    } else if (isdir(path) && !RemoveDirectoryA(path)) {
        throw system_exception("Failed to remove directory");
    }
}

/**
 * @brief 拼接两个路径，若第一个路径结尾没有分隔符，会添加一个分隔符
 * @return 拼接后的路径
 */
auto join(const char* path1, const char* path2) -> CString {
    auto len1 = std::strlen(path1);
    auto len2 = std::strlen(path2);
    if (len1 == 0) return CString(path2);
    if (len2 == 0) return CString(path1);

    // 检查是否需要分隔符
    bool needs_sep = true;
    char last_char = path1[len1 - 1];
    if (last_char == '\\' || last_char == '/') {
        needs_sep = false;
    }

    usize total_chars = len1 + len2 + (needs_sep ? 1 : 0);
    CString result(total_chars); // 分配 total_chars + 1 的内存

    char* str = result.data();
    std::memcpy(str, path1, len1);
    str += len1;

    if (needs_sep) {
        *str = PATH_SEP;
        str++;
    }

    std::memcpy(str, path2, len2);

    result[total_chars] = '\0';
    return result;
}

/**
 * @brief 列出文件夹中的文件名
 * @return 文件名集合
 */
auto listdir(const char* path) -> util::Vec<CString> {
    WIN32_FIND_DATAA find_data;
    auto handle = FindFirstFileA(join(path, "*"), &find_data);

    if (handle == INVALID_HANDLE_VALUE) {
        throw system_exception("Failed to list directory: {}", path);
    }

    util::Vec<CString> results;
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        results.push(find_data.cFileName);
    } while (FindNextFileA(handle, &find_data));

    FindClose(handle);
    return results;
}

} // namespace my::fs::win

#endif // WIN_FILE_UTILS_HPP