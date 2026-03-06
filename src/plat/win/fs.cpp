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

std::string to_std(const str::StringView view) {
    return std::string(reinterpret_cast<const char*>(view.as_bytes()), view.len());
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
    const auto p = to_std(path);
    const auto attributes = GetFileAttributesA(p.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES);
}

bool is_file(const str::StringView path) {
    if (path.len() == 0) {
        return false;
    }
    const auto p = to_std(path);
    const auto attr = GetFileAttributesA(p.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool is_dir(const str::StringView path) {
    if (path.len() == 0) {
        return false;
    }
    const auto p = to_std(path);
    const auto attr = GetFileAttributesA(p.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

void mkdir(const str::StringView path, bool recursive, bool exist_ok) {
    if (path.len() == 0) {
        throw argument_exception("Invalid path");
    }
    const auto path_s = to_std(path);

    if (!recursive) {
        mkdir_single(path_s.c_str(), exist_ok);
        return;
    }

    std::string p(path_s);
    while (!p.empty() && is_sep(p.back())) {
        p.pop_back();
    }
    if (p.empty()) {
        return;
    }
    if (p.size() == 2 && p[1] == ':') {
        return;
    }

    size_t start = 0;
    if (p.size() >= 2 && p[1] == ':') {
        start = 2;
        if (start < p.size() && is_sep(p[start])) {
            ++start;
        }
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

void remove(const str::StringView path, const bool recursive) {
    if (path.len() == 0) {
        throw argument_exception("Invalid path");
    }
    const auto path_s = to_std(path);
    if (!exists(path)) {
        throw not_found_exception("File or directory not found: {}", path_s);
    }

    if (is_file(path)) {
        if (!DeleteFileA(path_s.c_str())) {
            throw system_exception("Failed to remove file: {}", path_s);
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
        if (!RemoveDirectoryA(path_s.c_str())) {
            throw system_exception("Failed to remove directory: {}", path_s);
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

    const auto a_s = to_std(a);
    const auto b_s = to_std(b);
    const auto a_len = a_s.size();
    bool needs_sep = true;
    if (a_len > 0 && is_sep(a_s[a_len - 1])) {
        needs_sep = false;
    }

    std::string res;
    res.reserve(a_len + b_s.size() + (needs_sep ? 1 : 0));
    res.append(a_s);
    if (needs_sep) {
        res.push_back(PATH_SEP);
    }
    res.append(b_s);
    return str::String<>(res.c_str(), res.size());
}

util::Vec<DirEntry> listdir(const str::StringView path) {
    if (path.len() == 0) {
        throw argument_exception("Invalid path");
    }
    const auto path_s = to_std(path);

    WIN32_FIND_DATAA find_data;
    const auto pattern = join(path, str::StringView("*"));
    auto pattern_cstr = pattern.into_cstr();
    auto handle = FindFirstFileA(pattern_cstr.get(), &find_data);
    if (handle == INVALID_HANDLE_VALUE) {
        throw system_exception("Failed to list directory: {}", path_s);
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
    const auto path_s = to_std(path);
    const auto mode_s = to_std(mode);

#if defined(_MSC_VER)
    std::FILE* fp = nullptr;
    if (fopen_s(&fp, path_s.c_str(), mode_s.c_str()) != 0) {
        fp = nullptr;
    }
#else
    std::FILE* fp = std::fopen(path_s.c_str(), mode_s.c_str());
#endif
    if (fp == nullptr) {
        throw io_exception("Failed to open file: {}", path_s);
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
