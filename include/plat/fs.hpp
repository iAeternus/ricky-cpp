#ifndef PLAT_FS_HPP
#define PLAT_FS_HPP

#include "str.hpp"
#include "vec.hpp"

namespace my::plat::fs {

/**
 * @brief 用于 listdir 结果的文件条目信息
 */
struct DirEntry {
    util::String name;
    bool is_file{false};
    bool is_dir{false};
};

/**
 * @brief 不透明文件句柄
 */
struct FileHandle;

/**
 * @brief 路径是否存在
 */
bool exists(const char* path);

/**
 * @brief 是否为普通文件
 */
bool is_file(const char* path);

/**
 * @brief 是否为目录
 */
bool is_dir(const char* path);

/**
 * @brief 创建目录
 * @param recursive 是否递归创建
 */
void mkdir(const char* path, bool recursive);

/**
 * @brief 删除文件或目录（由实现决定是否递归）
 */
void remove(const char* path);

/**
 * @brief 路径拼接
 */
util::String join(const char* a, const char* b);

/**
 * @brief 列出目录项
 */
util::Vec<DirEntry> listdir(const char* path);

/**
 * @brief 打开文件，返回句柄（由实现分配）
 */
FileHandle* open(const char* path, const char* mode);

/**
 * @brief 读取整个文件内容
 */
util::String read_all(FileHandle* file);

/**
 * @brief 写入数据
 */
usize write(FileHandle* file, const char* data, usize size);

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
