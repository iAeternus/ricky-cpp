#include "path_buf.hpp"
#include "fs.hpp"

namespace my::fs {

namespace {

bool is_sep(const char ch) {
    return ch == '/' || ch == '\\';
}

bool is_drive_prefix(const std::string& path) {
    return path.size() >= 2 && std::isalpha(static_cast<unsigned char>(path[0])) && path[1] == ':';
}

size_t trim_trailing_seps(const std::string& path) {
    if (path.empty()) {
        return std::string::npos;
    }
    auto end = path.find_last_not_of("/\\");
    if (end == std::string::npos) {
        return std::string::npos;
    }
    return end;
}

std::string to_std(const str::String<>& s) {
    auto view = s.as_str();
    return std::string(reinterpret_cast<const char*>(view.as_bytes()), view.len());
}

} // namespace

PathBuf::PathBuf(const char* path) {
    path_ = str::String<>(path ? path : "");
}

PathBuf::PathBuf(const CString& path) {
    path_ = str::String<>(path.data(), path.length());
}

PathBuf::PathBuf(const str::String<>& path) :
        path_(path) {}

bool PathBuf::is_empty() const {
    return path_.is_empty();
}

usize PathBuf::len() const {
    return path_.len();
}

bool PathBuf::is_absolute() const {
    const auto s = to_std(path_);
    if (s.empty()) {
        return false;
    }
    if (is_sep(s.front())) {
        return true;
    }
    return is_drive_prefix(s);
}

bool PathBuf::is_relative() const {
    return !is_absolute();
}

PathBuf PathBuf::join(const char* other) const {
    auto base = as_cstr();
    auto joined = plat::fs::join(base.data(), other);
    auto c = joined.__str__();
    return PathBuf(str::String<>(c.data(), c.length()));
}

PathBuf PathBuf::join(const PathBuf& other) const {
    auto other_cstr = other.as_cstr();
    return join(other_cstr.data());
}

void PathBuf::push(const char* other) {
    auto joined = join(other);
    path_ = joined.path_;
}

void PathBuf::push(const PathBuf& other) {
    auto joined = join(other);
    path_ = joined.path_;
}

bool PathBuf::pop() {
    auto s = to_std(path_);
    if (s.empty()) {
        return false;
    }

    const auto end = trim_trailing_seps(s);
    if (end == std::string::npos) {
        path_ = str::String<>();
        return true;
    }

    const auto sep = s.find_last_of("/\\", end);
    if (sep == std::string::npos) {
        path_ = str::String<>();
        return true;
    }

    if (sep == 0) {
        s.resize(1);
        path_ = str::String<>(s.c_str(), s.size());
        return true;
    }

    if (sep == 2 && is_drive_prefix(s)) {
        s.resize(3);
        s[2] = '\\';
        path_ = str::String<>(s.c_str(), s.size());
        return true;
    }

    s.resize(sep);
    path_ = str::String<>(s.c_str(), s.size());
    return true;
}

str::String<> PathBuf::file_name() const {
    auto s = to_std(path_);
    const auto end = trim_trailing_seps(s);
    if (end == std::string::npos) {
        return str::String<>();
    }

    const auto sep = s.find_last_of("/\\", end);
    if (sep == std::string::npos) {
        return str::String<>(s.c_str(), end + 1);
    }
    return str::String<>(s.c_str() + sep + 1, end - sep);
}

str::String<> PathBuf::file_stem() const {
    auto name = to_std(file_name());
    if (name.empty()) {
        return str::String<>();
    }

    const auto dot = name.find_last_of('.');
    if (dot == std::string::npos || dot == 0) {
        return str::String<>(name.c_str(), name.size());
    }
    return str::String<>(name.c_str(), dot);
}

str::String<> PathBuf::extension() const {
    auto name = to_std(file_name());
    if (name.empty()) {
        return str::String<>();
    }

    const auto dot = name.find_last_of('.');
    if (dot == std::string::npos || dot == 0 || dot + 1 >= name.size()) {
        return str::String<>();
    }
    return str::String<>(name.c_str() + dot + 1, name.size() - dot - 1);
}

bool PathBuf::set_extension(const char* ext) {
    if (ext == nullptr) {
        return false;
    }

    auto s = to_std(path_);
    const auto end = trim_trailing_seps(s);
    if (end == std::string::npos) {
        return false;
    }

    const auto sep = s.find_last_of("/\\", end);
    const auto name_start = (sep == std::string::npos) ? 0 : sep + 1;
    if (name_start > end) {
        return false;
    }

    std::string base = s.substr(0, name_start);
    std::string filename = s.substr(name_start, end - name_start + 1);
    if (filename.empty()) {
        return false;
    }

    std::string ext_str(ext);
    if (!ext_str.empty() && ext_str.front() == '.') {
        ext_str.erase(ext_str.begin());
    }

    const auto dot = filename.find_last_of('.');
    if (dot != std::string::npos && dot != 0) {
        filename = filename.substr(0, dot);
    }

    if (!ext_str.empty()) {
        filename += ".";
        filename += ext_str;
    }

    base += filename;
    path_ = str::String<>(base.c_str(), base.size());
    return true;
}

PathBuf PathBuf::parent() const {
    PathBuf res(*this);
    res.pop();
    return res;
}

const str::String<>& PathBuf::as_string() const {
    return path_;
}

CString PathBuf::as_cstr() const {
    return CString(reinterpret_cast<const char*>(path_.as_bytes()), path_.len());
}

} // namespace my::fs
