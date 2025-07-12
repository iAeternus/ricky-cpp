/**
 * @brief 打印工具，将对象打印到文件
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef PRINTER_HPP
#define PRINTER_HPP

#include "Object.hpp"
#include "Color.hpp"

namespace my::io {

class Printer : public Object<Printer> {
public:
    Printer(std::FILE* file_ptr, CString sw = " ", CString ew = "\n") :
            ew_(std::move(ew)), sw_(std::move(sw)), output_file_(file_ptr) {}

    template <Printable T0, Printable... Args>
    void operator()(const T0& obj, const Args&... args) const {
        __print__(obj);
        if constexpr (sizeof...(args) > 0) {
            ((__print__(sw_), __print__(args)), ...);
        }
        __print__(ew_);
    }

    void operator()() const {
        __print__(ew_);
    }

    /**
     * @brief 设置输出结束符
     */
    void set_end(CString ew) {
        ew_ = std::move(ew);
    }

    /**
     * @brief 设置输出分隔符
     */
    void set_sep(CString sw) {
        sw_ = std::move(sw);
    }

protected:
    void __print__(const bool& obj) const {
        std::fprintf(output_file_, obj ? "true" : "false");
    }

    void __print__(const char& obj) const {
        std::fprintf(output_file_, "%c", obj);
    }

    void __print__(const char* obj) const {
        std::fprintf(output_file_, "%s", obj);
    }

    void __print__(std::nullptr_t) const {
        std::fprintf(output_file_, "nullptr");
    }

    void __print__(const std::string& obj) const {
        std::fprintf(output_file_, "%s", obj.c_str());
    }

    void __print__(const CString& obj) const {
        std::fprintf(output_file_, "%s", obj.data());
    }

    template <typename T>
    requires std::is_integral_v<T> void __print__(const T& obj) const {
        if constexpr (std::is_signed_v<T>) {
            std::fprintf(output_file_, "%lld", static_cast<long long>(obj));
        } else {
            std::fprintf(output_file_, "%llu", static_cast<unsigned long long>(obj));
        }
    }

    template <typename T>
    requires std::is_floating_point_v<T> void __print__(const T& obj) const {
        if constexpr (std::is_same_v<T, f32>) {
            std::fprintf(output_file_, "%f", obj);
        } else if constexpr (std::is_same_v<T, f64>) {
            std::fprintf(output_file_, "%lf", obj);
        } else if constexpr (std::is_same_v<T, long double>) {
            std::fprintf(output_file_, "%Lf", obj);
        }
    }

    template <typename T>
    requires std::is_pointer_v<T> void __print__(const T obj) const {
        std::fprintf(output_file_, "0x%p", static_cast<void*>(obj));
    }

    template <MyPrintable T>
    void __print__(const T& obj) const {
        __print__(obj.__str__());
    }

private:
    CString ew_;             // 结束符
    CString sw_;             // 分隔符
    std::FILE* output_file_; // 输出文件
};

class ColorPrinter : public Printer {
public:
    using Super = Printer;

    ColorPrinter(std::FILE* file_ptr, CString color = Color::WHITE) :
            Printer(file_ptr), color_(std::move(color)) {}

    template <Printable T, Printable... Args>
    void operator()(const T& obj, const Args&... args) const {
        opencolor();
        Super::operator()(obj, args...);
        closecolor();
    }

    void opencolor() const {
        Super::__print__(color_);
    }

    void closecolor() const {
        Super::__print__(Color::CLOSE);
    }

    void setcolor(CString color) {
        color_ = std::move(color);
    }

private:
    CString color_;
};

static Printer println{stdout};
static Printer print{stdout, " ", " "};
static ColorPrinter my_warner{stdin, Color::YELLOW};
static ColorPrinter my_error{stderr, Color::RED};

}; // namespace my::io

template <my::MyPrintable T>
struct std::formatter<T> : std::formatter<const char*> {
    auto format(const T& value, std::format_context& ctx) const {
        return std::formatter<const char*>::format(static_cast<const char*>(value.__str__()), ctx);
    }
};

#define tlog(expr) print(#expr, " = ", expr)

#endif // PRINTER_HPP