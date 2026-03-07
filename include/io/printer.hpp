/**
 * @brief 打印工具，将对象打印到文件
 * @author Ricky
 * @date 2024/11/27
 * @version 2.0
 */
#ifndef PRINTER_HPP
#define PRINTER_HPP

#include "object.hpp"
#include "color.hpp"

namespace my::io {

class Printer : public Object<Printer> {
public:
    Printer(std::FILE* file_ptr, CString sw = " ", CString ew = "\n") :
            ew_(std::move(ew)), sw_(std::move(sw)), output_file_(file_ptr) {}

    template <Printable T0, Printable... Args>
    auto operator()(const T0& obj, const Args&... args) const -> void {
        __print__(obj);
        if constexpr (sizeof...(args) > 0) {
            ((__print__(sw_), __print__(args)), ...);
        }
        __print__(ew_);
    }

    auto operator()() const -> void {
        __print__(ew_);
    }

    /**
     * @brief 设置输出结束符
     */
    auto set_end(CString ew) -> void {
        ew_ = std::move(ew);
    }

    /**
     * @brief 设置输出分隔符
     */
    auto set_sep(CString sw) -> void {
        sw_ = std::move(sw);
    }

protected:
    auto __print__(const bool& obj) const -> void {
        std::fprintf(output_file_, obj ? "true" : "false");
    }

    auto __print__(const char& obj) const -> void {
        std::fprintf(output_file_, "%c", obj);
    }

    auto __print__(const char* obj) const -> void {
        std::fprintf(output_file_, "%s", obj);
    }

    auto __print__(std::nullptr_t) const -> void {
        std::fprintf(output_file_, "nullptr");
    }

    auto __print__(const std::string& obj) const -> void {
        std::fprintf(output_file_, "%s", obj.c_str());
    }

    auto __print__(const CString& obj) const -> void {
        std::fprintf(output_file_, "%s", obj.data());
    }

    template <typename T>
        requires std::is_integral_v<T>
    auto __print__(const T& obj) const -> void {
        if constexpr (std::is_signed_v<T>) {
            std::fprintf(output_file_, "%lld", static_cast<long long>(obj));
        } else {
            std::fprintf(output_file_, "%llu", static_cast<unsigned long long>(obj));
        }
    }

    template <typename T>
        requires std::is_floating_point_v<T>
    auto __print__(const T& obj) const -> void {
        if constexpr (std::is_same_v<T, f32>) {
            std::fprintf(output_file_, "%f", obj);
        } else if constexpr (std::is_same_v<T, f64>) {
            std::fprintf(output_file_, "%lf", obj);
        } else if constexpr (std::is_same_v<T, long double>) {
            std::fprintf(output_file_, "%Lf", obj);
        }
    }

    template <typename T>
        requires std::is_pointer_v<T>
    auto __print__(const T obj) const -> void {
        std::fprintf(output_file_, "0x%p", static_cast<void*>(obj));
    }

    template <MyPrintable T>
    auto __print__(const T& obj) const -> void {
        auto s = std::format("{}", obj.to_string());
        std::fprintf(output_file_, "%s", s.c_str());
    }

private:
    CString ew_;             // 结束符
    CString sw_;             // 分隔符
    std::FILE* output_file_; // 输出文件
};

class ColorPrinter : public Printer {
public:
    using Super = Printer;

    ColorPrinter(std::FILE* file_ptr, CString color = color::Color::WHITE) :
            Printer(file_ptr), color_(std::move(color)) {}

    template <Printable T, Printable... Args>
    auto operator()(const T& obj, const Args&... args) const -> void {
        opencolor();
        Super::operator()(obj, args...);
        closecolor();
    }

    auto opencolor() const -> void {
        Super::__print__(color_);
    }

    auto closecolor() const -> void {
        Super::__print__(color::Color::CLOSE);
    }

    auto setcolor(CString color) -> void {
        color_ = std::move(color);
    }

private:
    CString color_;
};

static Printer println{stdout};
static Printer print{stdout, " ", " "};
static ColorPrinter my_warner{stdin, color::Color::YELLOW};
static ColorPrinter my_error{stderr, color::Color::RED};

}; // namespace my::io

template <my::MyPrintable T>
struct std::formatter<T> : std::formatter<std::string, char> {
    auto format(const T& value, std::format_context& ctx) const {
        auto s = std::format("{}", value.to_string());
        return std::formatter<std::string, char>::format(s, ctx);
    }
};

#define tlog(expr) print(#expr, " = ", expr)

#endif // PRINTER_HPP
