#include "sys.hpp"

#if RICKY_LINUX

#include "fs.hpp"
#include "vec.hpp"

#include <cerrno>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

namespace my::plat::fs {

struct FileHandle {
    std::FILE* fp{nullptr};
};

namespace {

static constexpr char PATH_SEP = '/';

bool is_sep(const char ch) {
    return ch == '/' || ch == '\\';
}

bool is_abs_path(const char* path) {
    if (path == nullptr) return false;
    return path[0] == '/' || (path[0] != '\0' && path[1] == ':');
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

bool exists(str::StringView path) {
    if (path.is_empty()) {
        return false;
    }
    struct stat st {};
    return ::stat(path.as_cstr(), &st) == 0;
}

bool is_file(str::StringView path) {
    if (path.is_empty()) {
        return false;
    }
    struct stat st {};
    if (::stat(path.as_cstr(), &st) != 0) {
        return false;
    }
    return S_ISREG(st.st_mode);
}

bool is_dir(str::StringView path) {
    if (path.is_empty()) {
        return false;
    }
    struct stat st {};
    if (::stat(path.as_cstr(), &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

void mkdir(str::StringView path, bool recursive, bool exist_ok) {
    if (path.is_empty()) {
        throw argument_exception("Invalid path");
    }

    if (!recursive) {
        mkdir_single(path.as_cstr(), exist_ok);
        return;
    }

    str::String<> p = path.to_string();
    while (!p.is_empty() && is_sep(p.last())) {
        p.pop();
    }
    if (p.is_empty()) {
        return;
    }

    size_t start = 0;
    if (p[0] == '/') {
        start = 1;
    }

    for (size_t i = start; i < p.len(); ++i) {
        if (is_sep(p[i])) {
            const auto sub = p.substr(0, i);
            if (!sub.is_empty()) {
                mkdir_single(sub.as_cstr(), true);
            }
        }
    }
    mkdir_single(p.as_cstr(), exist_ok);
}

void remove(str::StringView path, const bool recursive) {
    if (path.is_empty()) {
        throw argument_exception("Invalid path");
    }
    if (!exists(path)) {
        throw not_found_exception("File or directory not found: {}", path);
    }

    if (is_file(path)) {
        if (::unlink(path.as_cstr()) != 0) {
            throw system_exception("Failed to remove file: {}", path);
        }
        return;
    }

    if (is_dir(path)) {
        if (recursive) {
            const auto entries = listdir(path);
            for (const auto& entry : entries) {
                auto child = join(path, entry.name.as_str());
                remove(child.as_str(), true);
            }
        }
        if (::rmdir(path.as_cstr()) != 0) {
            throw system_exception("Failed to remove directory: {}", path);
        }
    }
}

str::String<> join(str::StringView a, str::StringView b) {
    if (a.is_empty()) {
        return str::String<>(b);
    }
    if (b.is_empty()) {
        return str::String<>(a);
    }
    const char* b_cstr = b.as_cstr();
    if (b_cstr != nullptr && is_abs_path(b_cstr)) {
        return str::String<>(b);
    }

    const auto a_len = a.len();
    bool needs_sep = true;
    if (a_len > 0 && is_sep(a[a_len - 1])) {
        needs_sep = false;
    }

    str::String res;
    res.reserve(a_len + b.len() + (needs_sep ? 1 : 0));
    for (usize i = 0; i < a_len; ++i) {
        char c = a[i];
        if (c == '\\') {
            c = '/';
        }
        res.push(c);
    }
    if (needs_sep) {
        res.push(PATH_SEP);
    }
    for (usize i = 0; i < b.len(); ++i) {
        char c = b[i];
        if (c == '\\') {
            c = '/';
        }
        res.push(c);
    }
    return res;
}

util::Vec<DirEntry> listdir(str::StringView path) {
    if (path.is_empty()) {
        throw argument_exception("Invalid path");
    }

    DIR* dir = ::opendir(path.as_cstr());
    if (dir == nullptr) {
        throw system_exception("Failed to list directory: {}", path);
    }

    util::Vec<DirEntry> results;
    while (auto* entry = ::readdir(dir)) {
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        DirEntry info{};
        info.name = str::String<>(entry->d_name);

        auto full_path = join(path, str::StringView(entry->d_name));
        struct stat st {};
        if (::stat(full_path.as_cstr(), &st) == 0) {
            info.is_dir = S_ISDIR(st.st_mode);
            info.is_file = S_ISREG(st.st_mode);
        }

        results.push(std::move(info));
    }

    ::closedir(dir);
    return results;
}

FileHandle* open(str::StringView path, str::StringView mode) {
    if (path.is_empty() || mode.is_empty()) {
        throw argument_exception("Invalid path or mode");
    }
    std::FILE* fp = std::fopen(path.as_cstr(), mode.as_cstr());
    if (fp == nullptr) {
        throw io_exception("Failed to open file: {}", path);
    }
    auto* handle = new FileHandle{};
    handle->fp = fp;
    return handle;
}

FileHandle* open(str::StringView path, const OpenMode mode) {
    return open(path, str::StringView(mode_to_cstr(mode)));
}

str::String<> read_all(FileHandle* file) {
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
        return str::String<>{};
    }

    util::Vec<char> buf(static_cast<size_t>(end));
    const size_t read_bytes = std::fread(buf.data(), 1, buf.len(), file->fp);
    if (read_bytes != buf.len() && std::ferror(file->fp)) {
        throw io_exception("Failed to read file");
    }
    return str::String<>(buf.data(), static_cast<usize>(read_bytes));
}

str::String<> read_all(str::StringView path) {
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

usize write(FileHandle* file, str::StringView data, usize size) {
    if (file == nullptr || file->fp == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    if (data.is_empty() && size > 0) {
        throw argument_exception("Invalid data pointer");
    }
    const size_t written = std::fwrite(data.as_cstr(), 1, static_cast<size_t>(size), file->fp);
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
