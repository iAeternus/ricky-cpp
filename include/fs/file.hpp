/**
 * @brief 文件
 * @author Ricky
 * @date 2026/2/17
 * @version 1.0
 */
#ifndef FILE_HPP
#define FILE_HPP

#include "marker.hpp"
#include "fs.hpp"
#include "str.hpp"
#include "path_buf.hpp"

namespace my::fs {

class File : public NoCopyMove {
public:
    using Handle = plat::fs::FileHandle;

    File(const char* path, plat::fs::OpenMode mode);
    File(const PathBuf& path, plat::fs::OpenMode mode);

    ~File();

    static File open(const char* path);
    static File open(const PathBuf& path);
    static File create(const char* path);
    static File create(const PathBuf& path);
    static File append(const char* path);
    static File append(const PathBuf& path);

    bool is_open() const;
    void close();

    util::String read_all();
    util::String read_all() const;

    usize write(const char* data, usize size);
    usize write(const CString& data);

    void flush();

    Handle* handle() const { return handle_; }

private:
    Handle* handle_{nullptr};
};

} // namespace my::fs

#endif // FILE_HPP
