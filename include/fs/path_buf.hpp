/**
 * @brief 路径
 * @author Ricky
 * @date 2026/2/17
 * @version 1.0
 */
#ifndef PATH_BUF_HPP
#define PATH_BUF_HPP

#include "str.hpp"

namespace my::fs {

class PathBuf : public Object<PathBuf> {
public:
    PathBuf() = default;
    explicit PathBuf(const char* path);
    explicit PathBuf(const CString& path);
    explicit PathBuf(const util::String& path);
    PathBuf(const PathBuf& other) = default;
    PathBuf(PathBuf&& other) noexcept = default;
    PathBuf& operator=(const PathBuf& other) = default;
    PathBuf& operator=(PathBuf&& other) noexcept = default;
    ~PathBuf() = default;

    bool is_empty() const;
    usize len() const;

    bool is_absolute() const;
    bool is_relative() const;

    PathBuf join(const char* other) const;
    PathBuf join(const PathBuf& other) const;

    void push(const char* other);
    void push(const PathBuf& other);

    bool pop();

    util::String file_name() const;
    util::String file_stem() const;
    util::String extension() const;
    bool set_extension(const char* ext);

    PathBuf parent() const;

    const util::String& as_string() const;
    CString as_cstr() const;
    CString __str__() const;

private:
    util::String path_{};
};

} // namespace my::fs

#endif // PATH_BUF_HPP
