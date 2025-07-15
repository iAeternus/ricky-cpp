/**
 * @brief 轻量级日志
 * @author Ricky
 * @date 2025/2/23
 * @version 1.0
 */
#ifndef LOG_HPP
#define LOG_HPP

#include "DateTime.hpp"
#include "Vec.hpp"
#include "Color.hpp"

#include <mutex>
#include <source_location>

namespace my::io {

/**
 * @brief 日志
 * @note 日志等级高于等于最低等级才会输出
 */
class Log : public Object<Log> {
public:
    using Self = Log;

    /**
     * @brief 日志级别
     */
    struct LogLevel {
        static constexpr i32 TRACE_ = 0;
        static constexpr i32 DEBUG_ = 1;
        static constexpr i32 INFO_ = 2;
        static constexpr i32 WARN_ = 3;
        static constexpr i32 ERROR_ = 4;
        static constexpr i32 FATAL_ = 5;
        static constexpr i32 INACTIVE_ = I32_MAX;

        static constexpr const char* LEVEL_NAMES[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
        static constexpr const char* LEVEL_COLORS[] = {Color::AQUA, Color::DEEPGREEN, Color::GREEN, Color::YELLOW, Color::RED, Color::PURPLE};
    };

    /**
     * @brief 日志处理器
     * @note 可以支持日志等级与输出文件间多对多的关系
     */
    class LogHandler : public Object<LogHandler> {
    public:
        constexpr LogHandler(i32 level = LogLevel::INACTIVE_, FILE* output = nullptr, bool enable_color = true) :
                level_(level), output_(output), enable_color_(enable_color) {}

        i32 level_;         // 日志等级
        FILE* output_;      // 指向输出文件的指针
        bool enable_color_; // 是否允许颜色打印
    };

    /**
     * @brief 添加日志配置
     */
    static void add_handler(const LogHandler& handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            handlers_.append(handler);
            if (handler.level_ < min_level_) {
                min_level_ = handler.level_;
            }
        } catch (...) {
            fprintf(stderr, "[ERROR] Failed to add log handler\n");
        }
    }

    /**
     * @brief 设置最低日志级别
     */
    static void set_level(i32 level) {
        std::lock_guard<std::mutex> lock(mutex_);
        min_level_ = level;
    }

    /**
     * @brief 打印trace日志
     * @param msg 日志消息
     * @param loc 日志位置
     */
    template <ConvertibleToCstr Str>
    static void trace(const Str& msg, std::source_location loc = SRC_LOC) {
        log_impl(msg, LogLevel::TRACE_, loc);
    }

    /**
     * @brief 打印trace日志
     * @param fmt 日志格式化字符串
     * @param loc 日志位置
     * @param args 格式化参数
     */
    template <typename... Args>
    static void trace(std::string_view fmt, std::source_location loc, Args&&... args) {
        auto msg = std::vformat(fmt, std::make_format_args(args...));
        log_impl(msg, LogLevel::TRACE_, loc);
    }

    /**
     * @brief 打印debug日志
     * @param msg 日志消息
     * @param loc 日志位置
     */
    template <ConvertibleToCstr Str>
    static void debug(const Str& msg, std::source_location loc = SRC_LOC) {
        log_impl(msg, LogLevel::DEBUG_, loc);
    }

    /**
     * @brief 打印debug日志
     * @param fmt 日志格式化字符串
     * @param loc 日志位置
     * @param args 格式化参数
     */
    template <typename... Args>
    static void debug(std::string_view fmt, std::source_location loc, Args&&... args) {
        auto msg = std::vformat(fmt, std::make_format_args(args...));
        log_impl(msg, LogLevel::DEBUG_, loc);
    }

    /**
     * @brief 打印info日志
     * @param msg 日志消息
     * @param loc 日志位置
     */
    template <ConvertibleToCstr Str>
    static void info(const Str& msg, std::source_location loc = SRC_LOC) {
        log_impl(msg, LogLevel::INFO_, loc);
    }

    /**
     * @brief 打印info日志
     * @param fmt 日志格式化字符串
     * @param loc 日志位置
     * @param args 格式化参数
     */
    template <typename... Args>
    static void info(std::string_view fmt, std::source_location loc, Args&&... args) {
        auto msg = std::vformat(fmt, std::make_format_args(args...));
        log_impl(msg, LogLevel::INFO_, loc);
    }

    /**
    * @brief 打印warn日志
    * @param msg 日志消息
    * @param loc 日志位置
    */
    template <ConvertibleToCstr Str>
    static void warn(const Str& msg, std::source_location loc = SRC_LOC) {
        log_impl(msg, LogLevel::WARN_, loc);
    }

    /**
     * @brief 打印warn日志
     * @param fmt 日志格式化字符串
     * @param loc 日志位置
     * @param args 格式化参数
     */
    template <typename... Args>
    static void warn(std::string_view fmt, std::source_location loc, Args&&... args) {
        auto msg = std::vformat(fmt, std::make_format_args(args...));
        log_impl(msg, LogLevel::WARN_, loc);
    }

    /**
     * @brief 打印error日志
     * @param msg 日志消息
     * @param loc 日志位置
     */
    template <ConvertibleToCstr Str>
    static void error(const Str& msg, std::source_location loc = SRC_LOC) {
        log_impl(msg, LogLevel::ERROR_, loc);
    }

    /**
     * @brief 打印error日志
     * @param fmt 日志格式化字符串
     * @param loc 日志位置
     * @param args 格式化参数
     */
    template <typename... Args>
    static void error(std::string_view fmt, std::source_location loc, Args&&... args) {
        auto msg = std::vformat(fmt, std::make_format_args(args...));
        log_impl(msg, LogLevel::ERROR_, loc);
    }

    /**
     * @brief 打印fatal日志
     * @param msg 日志消息
     * @param loc 日志位置
     */
    template <ConvertibleToCstr Str>
    static void fatal(const Str& msg, std::source_location loc = SRC_LOC) {
        log_impl(msg, LogLevel::FATAL_, loc);
    }

    /**
     * @brief 打印fatal日志
     * @param fmt 日志格式化字符串
     * @param loc 日志位置
     * @param args 格式化参数
     */
    template <typename... Args>
    static void fatal(std::string_view fmt, std::source_location loc, Args&&... args) {
        auto msg = std::vformat(fmt, std::make_format_args(args...));
        log_impl(msg, LogLevel::FATAL_, loc);
    }

private:
    static std::mutex mutex_;               // 互斥锁
    static util::Vec<LogHandler> handlers_; // 日志处理器
    static i32 min_level_;                  // 最低日志级别

    /**
     * @brief 打印日志
     * @param msg 日志消息
     * @param level 日志级别
     * @param loc 日志位置
     * @param date_time 日志时间
     * @details 它会根据日志级别和位置，选择合适的日志处理器来打印日志
     *          如果日志级别小于 min_level_，则什么也不做
     *          否则，根据当前配置快照（handlers_），选择合适的日志处理器来打印日志
     *          如果发现了合适的日志处理器，才真正打印日志
     */
    template <ConvertibleToCstr Str>
    static void log_impl(const Str msg, i32 level, std::source_location loc = SRC_LOC, util::DateTime date_time = util::DateTime::now()) {
        if (level < min_level_) return;

        // 获取当前配置快照
        util::Vec<LogHandler> local_handlers;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            local_handlers = handlers_;
        }

        // 通知所有级别等于给定级别的处理器打印日志
        for (const auto& handler : local_handlers) {
            if (handler.level_ == level) {
                print_log_handler(handler, msg, date_time, get_basename(loc.file_name()), loc.line());
            }
        }
    }

    /**
     * @brief 获取文件名的basename
     * @param path 文件名
     * @return 文件名的basename
     * @details 它会从给定的文件名中，提取出basename，basename是文件名中最后一个'/'或'\'之后的部分
     *          如果文件名中没有'/'或'\',那么整个文件名就是basename
     */
    static const char* get_basename(const char* path) {
        const char* slash = strrchr(path, '/');
        if (!slash) slash = strrchr(path, '\\');
        return slash ? slash + 1 : path;
    }

    /**
     * @brief 打印日志到指定的日志处理器
     * @param evt 日志处理器
     * @param msg 日志消息
     * @param date_time 日志时间
     * @param file_name 文件名
     * @param line 行号
     * @details 它会将日志消息写入到指定的日志处理器的输出流中
     *          如果日志处理器启用了颜色打印，那么日志级别和时间将被颜色化
     *          否则，它们将被打印成纯文本
     */
    static void print_log_handler(const LogHandler& evt, const char* msg, const util::DateTime& date_time, const char* file_name, i32 line) {
        fprintf(evt.output_, "%s %s%-5s%s %s:%d ",
                date_time.__str__().data(),
                evt.enable_color_ ? LogLevel::LEVEL_COLORS[evt.level_] : Color::CLOSE,
                LogLevel::LEVEL_NAMES[evt.level_],
                Color::CLOSE,
                file_name, line);
        fprintf(evt.output_, "%s", msg);
        fprintf(evt.output_, "\n");
        fflush(evt.output_);
    }
};

inline std::mutex Log::mutex_;
inline util::Vec<Log::LogHandler> Log::handlers_{
    Log::LogHandler(Log::LogLevel::TRACE_, stdout),
    Log::LogHandler(Log::LogLevel::DEBUG_, stdout),
    Log::LogHandler(Log::LogLevel::INFO_, stdout),
    Log::LogHandler(Log::LogLevel::WARN_, stdout),
    Log::LogHandler(Log::LogLevel::ERROR_, stdout),
    Log::LogHandler(Log::LogLevel::FATAL_, stdout),
};
inline i32 Log::min_level_ = Log::LogLevel::INFO_;

/**
 * @brief 打印异常日志
 * @param msg 异常消息
 * @param loc 异常位置
 * @details 它会将msg打印到error日志中，并带上loc的信息
 */
fn log_exception(const char* msg, std::source_location loc)->void {
    io::Log::error(msg, loc);
}

} // namespace my::io

#endif // LOG_HPP