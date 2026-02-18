#include "file.hpp"
#include "my_exception.hpp"

namespace my::fs {

File::File(const char* path, plat::fs::OpenMode mode) {
    fd_ = plat::fs::open(path, mode);
}

File::File(const PathBuf& path, plat::fs::OpenMode mode) {
    auto cstr = path.as_cstr();
    fd_ = plat::fs::open(cstr.data(), mode);
}

File::~File() {
    close();
}

File File::open(const char* path) {
    return File(path, plat::fs::OpenMode::ReadBinary);
}

File File::open(const PathBuf& path) {
    return File(path, plat::fs::OpenMode::ReadBinary);
}

File File::create(const char* path) {
    return File(path, plat::fs::OpenMode::WriteBinary);
}

File File::create(const PathBuf& path) {
    return File(path, plat::fs::OpenMode::WriteBinary);
}

File File::append(const char* path) {
    return File(path, plat::fs::OpenMode::AppendBinary);
}

File File::append(const PathBuf& path) {
    return File(path, plat::fs::OpenMode::AppendBinary);
}

bool File::is_open() const {
    return fd_ != nullptr;
}

void File::close() {
    if (fd_ == nullptr) {
        return;
    }
    plat::fs::close(fd_);
    fd_ = nullptr;
}

util::String File::read_all() {
    if (fd_ == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    return plat::fs::read_all(fd_);
}

util::String File::read_all() const {
    if (fd_ == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    return plat::fs::read_all(fd_);
}

usize File::write(const char* data, usize size) {
    if (fd_ == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    return plat::fs::write(fd_, data, size);
}

usize File::write(const CString& data) {
    return write(data.data(), data.length());
}

void File::flush() {
    if (fd_ == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    plat::fs::flush(fd_);
}

} // namespace my::fs
