/**
 * @brief win文件
 * @author Ricky
 * @date 2025/1/1
 * @version 1.0
 */
#ifndef WIN_FILE_HPP
#define WIN_FILE_HPP

#include "file_utils.hpp"
#include "NoCopy.hpp"

#include <winbase.h>

namespace my::fs::win {

class File : public Object<File>, public NoCopy {
    using Self = File;

public:
    File(const char* filename, CString mode) {
        i32 dwDesiredAccess = 0, dwCreationDisposition = 0;
        if (mode == cstr("w")) {
            dwDesiredAccess = GENERIC_WRITE;
            dwCreationDisposition = CREATE_ALWAYS;
        } else if (mode == cstr("r")) {
            dwDesiredAccess = GENERIC_READ;
            dwCreationDisposition = OPEN_EXISTING;
        } else if (mode == cstr("a")) {
            dwDesiredAccess = FILE_APPEND_DATA;
            dwCreationDisposition = OPEN_ALWAYS;
            SetFilePointer(fh_, 0, nullptr, FILE_END);
        } else {
            ValueError(std::format("Invalid value {}, that only support [w, r, a]", mode));
        }

        fh_ = CreateFileA(
            filename,
            dwDesiredAccess,
            FILE_SHARE_READ,
            nullptr,
            dwCreationDisposition,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

        if (fh_ == INVALID_HANDLE_VALUE) {
            SystemError("Invalid HANDLE value, Failed to create or open file");
        }
    }

    File(Self&& other) noexcept :
            fh_(other.fh_) {
        other.fh_ = nullptr;
    }

    ~File() {
        close();
    }

    void close() {
        CloseHandle(fh_);
        fh_ = nullptr;
    }

    void write(const char* c) const {
        DWORD written_bytes = 0;
        if (!WriteFile(fh_, c, strlen(c), &written_bytes, nullptr)) {
            SystemError("Failed to write from file");
        }
    }

    CString read() const {
        DWORD read_bytes = 0;
        usize buffer_size = fileSize() + 1;
        CString buffer{buffer_size};

        if (!ReadFile(fh_, buffer.data(), buffer_size, &read_bytes, nullptr))
            SystemError("Failed to read from file");

        return buffer;
    }

    usize fileSize() const {
        return GetFileSize(fh_, nullptr);
    }

private:
    HANDLE fh_;
};

} // namespace my::fs::win

#endif // WIN_FILE_HPP