#ifndef WIN_FILE_UTILS_HPP
#define WIN_FILE_UTILS_HPP

#include "winlib.hpp"
#include "raise_error.hpp"

namespace my::fs::win {

static constexpr char PATH_SEP = '\\';

/**
 * @brief 判断文件或文件夹是否存在
 * @return true=存在 false=不存在
 */
def exists(const char* path)->bool {
    DWORD attributes = GetFileAttributesA(path);
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

/**
 * @brief 判断是否为文件
 * @return true=是 false=否
 */
def isfile(const char* path)->bool {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

/**
 * @brief 判断是否为文件夹
 * @return true=是 false=否
 */
def isdir(const char* path)->bool {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

/**
 * @brief 创建一个文件夹
 * @return void
 */
def mkdir(const char* path, bool exist_ok = false) {
    BOOL state = CreateDirectoryA(path, nullptr);

    switch (state) {
    case ERROR_ALREADY_EXISTS:
        if (!exist_ok) {
            RuntimeError(std::format("Directory already exists: {}", path));
        }
        break;
    case ERROR_PATH_NOT_FOUND:
        FileNotFoundError(std::format("Path not found: {}", path));
        break;
    }
}

/**
 * @brief 删除一个文件或文件夹
 * @return void
 */
def remove(const char* path) {
    if (!exists(path)) {
        FileNotFoundError(std::format("File or Directory not found in {}", path));
    }

    if (isfile(path) && !DeleteFileA(path)) {
        SystemError("Failed to remove file");
    } else if (isdir(path) && !RemoveDirectoryA(path)) {
        SystemError("Failed to remove directory");
    }
}

/**
 * @brief 拼接两个路径，若第一个路径结尾没有分隔符，会添加一个分隔符
 * @return 拼接后的路径
 */
def join(const char* path1, const char* path2)->CString {
    c_size length1 = std::strlen(path1);
    c_size length2 = std::strlen(path2);
    c_size length = length1 + length2 + (path1[length1 - 1] != '\\' && path1[length1 - 1] != '/');

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
def listdir(const char* path)->util::DynArray<CString> {
    WIN32_FIND_DATAA find_data;
    HANDLE handle = FindFirstFileA(join(path, "*"), &find_data);

    if (handle == INVALID_HANDLE_VALUE) {
        SystemError(std::format("Failed to list directory: {}", path));
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