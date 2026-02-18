#include "my_config.hpp"

#if RICKY_LINUX

#include "fs.hpp"
#include "str.hpp"
#include "vec.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace my::plat::fs {

struct FileHandle {
    std::FILE* fp{nullptr};
};

namespace {

static constexpr char PATH_SEP = '/';

bool is_sep(const char ch) {
    return ch == '/';
}

bool is_abs_path(const char* path) {
    return path != nullptr && path[0] == '/';
}

const char* mode_to_cstr(const OpenMode mode) {
    switch (mode) {
    case OpenMode::Read: return "r";
    case OpenMode::Write: return "w";
    case OpenMode::Append: return "a";
    case OpenMode::ReadBinary: return "rb";
    case OpenMode::WriteBinary: return "wb";
    case OpenMode::AppendBinary: return "ab";
    default: return "r";
    }
}

void mkdir_single(const char* path, const bool exist_ok) {
    if (::mkdir(path, 0755) == 0) {
        return;
    }
    if (errno == EEXIST) {
        if (!exist_ok) {
            throw runtime_exception("Directory already exists: {}", path);
        }
        return;
    }
    throw system_exception("Failed to create directory: {}", path);
}

} // namespace

bool exists(const char* path) {
    if (path == nullptr || path[0] == '\0') {
        return false;
    }
    struct stat st {};
    return ::stat(path, &st) == 0;
}

bool is_file(const char* path) {
    if (path == nullptr || path[0] == '\0') {
        return false;
    }
    struct stat st {};
    if (::stat(path, &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode);
}

bool is_dir(const char* path) {
    if (path == nullptr || path[0] == '\0') {
        return false;
    }
    struct stat st {};
    if (::stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

void mkdir(const char* path, bool recursive, bool exist_ok) {
    if (path == nullptr || path[0] == '\0') {
        throw argument_exception("Invalid path");
    }

    if (!recursive) {
        mkdir_single(path, exist_ok);
        return;
    }

    std::string p(path);
    while (!p.empty() && is_sep(p.back())) {
        p.pop_back();
    }
    if (p.empty()) {
        return;
    }

    size_t start = 0;
    if (p[0] == '/') {
        start = 1;
    }

    for (size_t i = start; i < p.size(); ++i) {
        if (is_sep(p[i])) {
            const auto sub = p.substr(0, i);
            if (!sub.empty()) {
                mkdir_single(sub.c_str(), true);
            }
        }
    }
    mkdir_single(p.c_str(), exist_ok);
}

void remove(const char* path, const bool recursive) {
    if (path == nullptr || path[0] == '\0') {
        throw argument_exception("Invalid path");
    }
    if (!exists(path)) {
        throw not_found_exception("File or directory not found: {}", path);
    }

    if (is_file(path)) {
        if (::unlink(path) != 0) {
            throw system_exception("Failed to remove file: {}", path);
        }
        return;
    }

    if (is_dir(path)) {
        if (recursive) {
            const auto entries = listdir(path);
            for (const auto& entry : entries) {
                auto child = join(path, entry.name.__str__().data());
                remove(child.__str__().data(), true);
            }
        }
        if (::rmdir(path) != 0) {
            throw system_exception("Failed to remove directory: {}", path);
        }
    }
}

util::String join(const char* a, const char* b) {
    if (a == nullptr || a[0] == '\0') {
        return util::String(b ? b : "");
    }
    if (b == nullptr || b[0] == '\0') {
        return util::String(a);
    }
    if (is_abs_path(b)) {
        return util::String(b);
    }

    const auto a_len = std::strlen(a);
    bool needs_sep = true;
    if (a_len > 0 && is_sep(a[a_len - 1])) {
        needs_sep = false;
    }

    std::string res;
    res.reserve(a_len + std::strlen(b) + (needs_sep ? 1 : 0));
    res.append(a);
    if (needs_sep) {
        res.push_back(PATH_SEP);
    }
    res.append(b);
    return util::String(res.c_str(), res.size());
}

util::Vec<DirEntry> listdir(const char* path) {
    if (path == nullptr || path[0] == '\0') {
        throw argument_exception("Invalid path");
    }

    DIR* dir = ::opendir(path);
    if (dir == nullptr) {
        throw system_exception("Failed to list directory: {}", path);
    }

    util::Vec<DirEntry> results;
    while (auto* entry = ::readdir(dir)) {
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        DirEntry info{};
        info.name = util::String(entry->d_name);

        auto full_path = join(path, entry->d_name);
        struct stat st {};
        if (::stat(full_path.__str__().data(), &st) == 0) {
            info.is_dir = S_ISDIR(st.st_mode);
            info.is_file = S_ISREG(st.st_mode);
        }

        results.push(std::move(info));
    }

    ::closedir(dir);
    return results;
}

FileHandle* open(const char* path, const char* mode) {
    if (path == nullptr || mode == nullptr) {
        throw argument_exception("Invalid path or mode");
    }
    std::FILE* fp = std::fopen(path, mode);
    if (fp == nullptr) {
        throw io_exception("Failed to open file: {}", path);
    }
    auto* handle = new FileHandle{};
    handle->fp = fp;
    return handle;
}

FileHandle* open(const char* path, const OpenMode mode) {
    return open(path, mode_to_cstr(mode));
}

util::String read_all(FileHandle* file) {
    if (file == nullptr || file->fp == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }

    if (std::fseek(file->fp, 0, SEEK_END) != 0) {
        throw io_exception("Failed to seek file");
    }
    const long end = std::ftell(file->fp);
    if (end < 0) {
        throw io_exception("Failed to get file size");
    }
    std::rewind(file->fp);

    if (end == 0) {
        return util::String{};
    }

    std::vector<char> buffer(static_cast<size_t>(end));
    const size_t read_bytes = std::fread(buffer.data(), 1, buffer.size(), file->fp);
    if (read_bytes != buffer.size() && std::ferror(file->fp)) {
        throw io_exception("Failed to read file");
    }
    return util::String(buffer.data(), static_cast<usize>(read_bytes));
}

util::String read_all(const char* path) {
    auto* file = open(path, OpenMode::ReadBinary);
    try {
        auto content = read_all(file);
        close(file);
        return content;
    } catch (...) {
        close(file);
        throw;
    }
}

usize write(FileHandle* file, const char* data, usize size) {
    if (file == nullptr || file->fp == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    if (data == nullptr && size > 0) {
        throw argument_exception("Invalid data pointer");
    }
    const size_t written = std::fwrite(data, 1, static_cast<size_t>(size), file->fp);
    if (written != size && std::ferror(file->fp)) {
        throw io_exception("Failed to write file");
    }
    return static_cast<usize>(written);
}

void flush(FileHandle* file) {
    if (file == nullptr || file->fp == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    if (std::fflush(file->fp) != 0) {
        throw io_exception("Failed to flush file");
    }
}

void close(FileHandle* file) {
    if (file == nullptr) {
        return;
    }
    if (file->fp != nullptr) {
        std::fclose(file->fp);
        file->fp = nullptr;
    }
    delete file;
}

} // namespace my::plat::fs

#endif // RICKY_LINUX
