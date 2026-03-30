#ifndef PLAT_FS_HPP
#define PLAT_FS_HPP

#include "string.hpp"
#include "vec.hpp"

namespace my::plat::fs {

/**
 * @brief 用于 listdir 结果的文件条目信息
 */
struct DirEntry {
    str::String<> name;
    bool is_file{false};
    bool is_dir{false};
};

/**
 * @brief 不透明文件句柄
 */
struct FileHandle;

/**
 * @brief 文件打开模式
 */
enum class OpenMode {
    Read,
    Write,
    Append,
    ReadBinary,
    WriteBinary,
    AppendBinary
};

/**
 * @brief 路径是否存在
 */
bool exists(str::StringView path);

/**
 * @brief 是否为普通文件
 */
bool is_file(str::StringView path);

/**
 * @brief 是否为目录
 */
bool is_dir(str::StringView path);

/**
 * @brief 创建目录
 * @param recursive 是否递归创建
 * @param exist_ok 目录已存在时是否视为成功
 */
void mkdir(str::StringView path, bool recursive = false, bool exist_ok = true);

/**
 * @brief 删除文件或目录
 * @param recursive 是否递归删除目录
 */
void remove(str::StringView path, bool recursive = false);

/**
 * @brief 路径拼接
 */
str::String<> join(str::StringView a, str::StringView b);

/**
 * @brief 列出目录项
 */
util::Vec<DirEntry> listdir(str::StringView path);

/**
 * @brief 打开文件，返回句柄（由实现分配）
 */
FileHandle* open(str::StringView path, str::StringView mode);

/**
 * @brief 打开文件，返回句柄
 */
FileHandle* open(str::StringView path, OpenMode mode);

/**
 * @brief 读取整个文件内容
 */
str::String<> read_all(FileHandle* file);

/**
 * @brief 读取整个文件内容（按路径）
 */
str::String<> read_all(str::StringView path);

/**
 * @brief 写入数据
 */
usize write(FileHandle* file, str::StringView data, usize size);

/**
 * @brief 刷新写缓冲
 */
void flush(FileHandle* file);

/**
 * @brief 关闭并释放句柄
 */
void close(FileHandle* file);

} // namespace my::plat::fs

#endif // PLAT_FS_HPP
