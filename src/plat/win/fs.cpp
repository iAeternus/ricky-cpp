#include "my_config.hpp"

#if RICKY_WIN

#include "fs.hpp"

#include <Windows.h>

namespace my::plat::fs {

struct FileHandle {
    std::FILE* fp{nullptr};
};

namespace {

static constexpr char PATH_SEP = '\\';

bool is_sep(const char ch) {
    return ch == '\\' || ch == '/';
}

bool is_abs_path(const str::StringView path) {
    if (path.len() == 0) {
        return false;
    }
    const auto* bytes = reinterpret_cast<const char*>(path.as_bytes());
    if (is_sep(bytes[0])) {
        return true;
    }
    return (path.len() >= 2) && (bytes[1] == ':');
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
    if (CreateDirectoryA(path, nullptr)) {
        return;
    }
    const auto err = GetLastError();
    if (err == ERROR_ALREADY_EXISTS) {
        if (!exist_ok) {
            throw runtime_exception("Directory already exists: {}", path);
        }
        return;
    }
    throw system_exception("Failed to create directory: {}", path);
}

} // namespace

bool exists(const str::StringView path) {
    if (path.len() == 0) {
        return false;
    }
    const auto path_cstr = path.into_cstr();
    const auto attributes = GetFileAttributesA(path_cstr.get());
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

bool is_file(const str::StringView path) {
    if (path.len() == 0) {
        return false;
    }
    const auto path_cstr = path.into_cstr();
    const auto attr = GetFileAttributesA(path_cstr.get());
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool is_dir(const str::StringView path) {
    if (path.len() == 0) {
        return false;
    }
    const auto path_cstr = path.into_cstr();
    const auto attr = GetFileAttributesA(path_cstr.get());
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

void mkdir(const str::StringView path, bool recursive, bool exist_ok) {
    if (path.len() == 0) {
        throw argument_exception("Invalid path");
    }

    if (!recursive) {
        const auto path_cstr = path.into_cstr();
        mkdir_single(path_cstr.get(), exist_ok);
        return;
    }

    str::String<> p = str::String<>(path);
    while (!p.is_empty() && is_sep(p.last())) {
        p.pop();
    }
    if (p.is_empty()) {
        return;
    }
    if (p.len() == 2 && p.chars().nth(1).unwrap() == ':') {
        return;
    }

    usize start = 0;
    if (p.len() >= 2 && p.chars().nth(1).unwrap() == ':') {
        start = 2;
        if (start < p.len() && is_sep(p.chars().nth(start).unwrap())) {
            ++start;
        }
    }
    for (usize i = start; i < p.len(); ++i) {
        if (is_sep(p.chars().nth(i).unwrap())) {
            const auto sub = p.slice(0, i);
            if (!sub.is_empty()) {
                const auto sub_cstr = sub.into_cstr();
                mkdir_single(sub_cstr.get(), true);
            }
        }
    }
    const auto p_cstr = p.into_cstr();
    mkdir_single(p_cstr.get(), exist_ok);
}

void remove(const str::StringView path, const bool recursive) {
    if (path.len() == 0) {
        throw argument_exception("Invalid path");
    }
    if (!exists(path)) {
        throw not_found_exception("File or directory not found: {}", path);
    }

    if (is_file(path)) {
        const auto path_cstr = path.into_cstr();
        if (!DeleteFileA(path_cstr.get())) {
            throw system_exception("Failed to remove file: {}", path);
        }
        return;
    }

    if (is_dir(path)) {
        if (recursive) {
            const auto entries = listdir(path);
            for (const auto& entry : entries) {
                const auto child = join(path, entry.name.as_str());
                remove(child.as_str(), true);
            }
        }
        const auto path_cstr = path.into_cstr();
        if (!RemoveDirectoryA(path_cstr.get())) {
            throw system_exception("Failed to remove directory: {}", path);
        }
    }
}

str::String<> join(const str::StringView a, const str::StringView b) {
    if (a.len() == 0) {
        return str::String<>(b);
    }
    if (b.len() == 0) {
        return str::String<>(a);
    }
    if (is_abs_path(b)) {
        return str::String<>(b);
    }

    const auto a_len = a.len();
    bool needs_sep = true;
    if (a_len > 0 && is_sep(a[a_len - 1])) {
        needs_sep = false;
    }

    str::String res;
    res.reserve(a_len + b.len() + (needs_sep ? 1 : 0));
    res.push_str(a);
    if (needs_sep) {
        res.push(PATH_SEP);
    }
    for (usize i = 0; i < b.len(); ++i) {
        char c = b[i];
        if (c == '/') {
            c = '\\';
        }
        res.push(c);
    }
    return res;
}

util::Vec<DirEntry> listdir(const str::StringView path) {
    if (path.len() == 0) {
        throw argument_exception("Invalid path");
    }

    WIN32_FIND_DATAA find_data;
    const auto pattern = join(path, str::StringView("*"));
    const auto pattern_cstr = pattern.into_cstr();
    auto handle = FindFirstFileA(pattern_cstr.get(), &find_data);
    if (handle == INVALID_HANDLE_VALUE) {
        throw system_exception("Failed to list directory: {}", path);
    }

    util::Vec<DirEntry> results;
    do {
        if (std::strcmp(find_data.cFileName, ".") == 0 || std::strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        DirEntry entry{};
        entry.name = str::String<>(find_data.cFileName);
        entry.is_dir = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        entry.is_file = !entry.is_dir;
        results.push(std::move(entry));
    } while (FindNextFileA(handle, &find_data));

    FindClose(handle);
    return results;
}

FileHandle* open(const str::StringView path, const str::StringView mode) {
    if (path.len() == 0 || mode.len() == 0) {
        throw argument_exception("Invalid path or mode");
    }

    const auto path_cstr = path.into_cstr();
    const auto mode_cstr = mode.into_cstr();
#if defined(_MSC_VER)
    std::FILE* fp = nullptr;
    if (fopen_s(&fp, path_cstr.get(), mode_cstr.get()) != 0) {
        fp = nullptr;
    }
#else
    std::FILE* fp = std::fopen(path_cstr.get(), mode_cstr.get());
#endif
    if (fp == nullptr) {
        throw io_exception("Failed to open file: {}", path);
    }
    auto* handle = new FileHandle{};
    handle->fp = fp;
    return handle;
}

FileHandle* open(const str::StringView path, const OpenMode mode) {
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

    std::vector<char> buffer(static_cast<size_t>(end));
    const size_t read_bytes = std::fread(buffer.data(), 1, buffer.size(), file->fp);
    if (read_bytes != buffer.size() && std::ferror(file->fp)) {
        throw io_exception("Failed to read file");
    }
    return str::String<>(buffer.data(), static_cast<usize>(read_bytes));
}

str::String<> read_all(const str::StringView path) {
    auto* file = open(path, OpenMode::ReadBinary);
    try {
        const auto content = read_all(file);
        close(file);
        return content;
    } catch (...) {
        close(file);
        throw;
    }
}

usize write(FileHandle* file, const str::StringView data, const usize size) {
    if (file == nullptr || file->fp == nullptr) {
        throw null_pointer_exception("Invalid file handle");
    }
    if (size > data.len()) {
        throw argument_exception("Write size exceeds data length");
    }
    const auto* bytes = reinterpret_cast<const char*>(data.as_bytes());
    const size_t written = std::fwrite(bytes, 1, static_cast<size_t>(size), file->fp);
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

#endif // RICKY_WIN
