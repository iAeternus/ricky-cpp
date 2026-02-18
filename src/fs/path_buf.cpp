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

} // namespace

PathBuf::PathBuf(const char* path) {
    path_ = util::String(path ? path : "");
}

PathBuf::PathBuf(const CString& path) {
    path_ = util::String(path.data(), path.length());
}

PathBuf::PathBuf(const util::String& path) :
        path_(path) {}

bool PathBuf::is_empty() const {
    return path_.is_empty();
}

usize PathBuf::len() const {
    return path_.len();
}

bool PathBuf::is_absolute() const {
    const auto s = path_.into_string();
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
    auto base = path_.__str__();
    return PathBuf(plat::fs::join(base.data(), other));
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
    auto s = path_.into_string();
    if (s.empty()) {
        return false;
    }

    const auto end = trim_trailing_seps(s);
    if (end == std::string::npos) {
        path_ = util::String();
        return true;
    }

    const auto sep = s.find_last_of("/\\", end);
    if (sep == std::string::npos) {
        path_ = util::String();
        return true;
    }

    if (sep == 0) {
        s.resize(1);
        path_ = util::String(s.c_str(), s.size());
        return true;
    }

    if (sep == 2 && is_drive_prefix(s)) {
        s.resize(3);
        s[2] = '\\';
        path_ = util::String(s.c_str(), s.size());
        return true;
    }

    s.resize(sep);
    path_ = util::String(s.c_str(), s.size());
    return true;
}

util::String PathBuf::file_name() const {
    auto s = path_.into_string();
    const auto end = trim_trailing_seps(s);
    if (end == std::string::npos) {
        return util::String();
    }

    const auto sep = s.find_last_of("/\\", end);
    if (sep == std::string::npos) {
        return util::String(s.c_str(), end + 1);
    }
    return util::String(s.c_str() + sep + 1, end - sep);
}

util::String PathBuf::file_stem() const {
    auto name = file_name().into_string();
    if (name.empty()) {
        return util::String();
    }

    const auto dot = name.find_last_of('.');
    if (dot == std::string::npos || dot == 0) {
        return util::String(name.c_str(), name.size());
    }
    return util::String(name.c_str(), dot);
}

util::String PathBuf::extension() const {
    auto name = file_name().into_string();
    if (name.empty()) {
        return util::String();
    }

    const auto dot = name.find_last_of('.');
    if (dot == std::string::npos || dot == 0 || dot + 1 >= name.size()) {
        return util::String();
    }
    return util::String(name.c_str() + dot + 1, name.size() - dot - 1);
}

bool PathBuf::set_extension(const char* ext) {
    if (ext == nullptr) {
        return false;
    }

    auto s = path_.into_string();
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
    path_ = util::String(base.c_str(), base.size());
    return true;
}

PathBuf PathBuf::parent() const {
    PathBuf res(*this);
    res.pop();
    return res;
}

const util::String& PathBuf::as_string() const {
    return path_;
}

CString PathBuf::as_cstr() const {
    return path_.__str__();
}

CString PathBuf::__str__() const {
    return path_.__str__();
}

} // namespace my::fs
