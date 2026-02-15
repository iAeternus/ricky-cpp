/**
 * @brief 日志
 * @author Ricky
 * @date 2025/10/1
 * @version 1.0
 */

#ifndef LOG_HPP
#define LOG_HPP

#include "my_config.hpp"
#include "my_exception.hpp"
#include "color.hpp"
#include "marker.hpp"
#include "printer.hpp"
#include "date_time.hpp"

#if RICKY_WIN
#include "processthreadsapi.h"
#else
#include <sys/types.h>
#include <unistd.h>
#endif // RICKY_WIN

namespace my::log {

/**
 * @brief 日志级别
 */
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal,
};

/**
 * @brief 日志级别包装器，提供类型转换（字符串或颜色）
 */
class LogLevelWrapper : public Object<LogLevelWrapper> {
public:
    using Self = LogLevelWrapper;

    LogLevelWrapper(const LogLevel level) :
            level_(level) {}

    const char* color() const {
        switch (level_) {
        case LogLevel::Trace: return color::Color::CYAN;
        case LogLevel::Debug: return color::Color::BLUE;
        case LogLevel::Info: return color::Color::GREEN;
        case LogLevel::Warn: return color::Color::YELLOW;
        case LogLevel::Error: return color::Color::RED;
        case LogLevel::Fatal: return color::Color::PURPLE;
        default: throw argument_exception("Unknown log level");
        }
    }

    CString __str__() const {
        switch (level_) {
        case LogLevel::Trace: return "Trace";
        case LogLevel::Debug: return "Debug";
        case LogLevel::Info: return "Info";
        case LogLevel::Warn: return "Warn";
        case LogLevel::Error: return "Error";
        case LogLevel::Fatal: return "Fatal";
        default: throw argument_exception("Unknown log level");
        }
    }

private:
    LogLevel level_;
};

/**
 * @brief 重置控制台颜色
 */
inline auto reset_color() noexcept -> const char* {
    return color::Color::CLOSE;
}

/**
 * @brief 日志记录
 */
struct LogRecord {
    util::DateTime datetime; // 日志记录时间
    u32 pid;                 // 进程ID
    const char* file_name;   // 文件名
    usize line;              // 行号
    std::string log;         // 日志内容
};

/**
 * @brief 获取当前pid
 */
inline auto get_current_pid() -> u32 {
#if RICKY_WIN
    return static_cast<u32>(GetCurrentProcessId());
#else
    return static_cast<u32>(getpid());
#endif // RICKY_WIN
}

template <typename D>
class BasicLogger : public Object<BasicLogger<D>>, public NoCopy {
public:
    using Self = BasicLogger<D>;

    void set_level(LogLevel level) {
        level_ = level;
    }

    [[nodiscard]] LogLevel level() const {
        return level_;
    }

    template <typename... Args>
    void trace(my::format_string_wrapper<Args...> fmt_w, Args&&... args) {
        format<LogLevel::Trace>(fmt_w, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(my::format_string_wrapper<Args...> fmt_w, Args&&... args) {
        format<LogLevel::Debug, Args...>(fmt_w, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(my::format_string_wrapper<Args...> fmt_w, Args&&... args) {
        format<LogLevel::Info>(fmt_w, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(my::format_string_wrapper<Args...> fmt_w, Args&&... args) {
        format<LogLevel::Warn, Args...>(fmt_w, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(my::format_string_wrapper<Args...> fmt_w, Args&&... args) {
        format<LogLevel::Error, Args...>(fmt_w, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void fatal(my::format_string_wrapper<Args...> fmt_w, Args&&... args) {
        format<LogLevel::Fatal>(fmt_w, std::forward<Args>(args)...);
    }

private:
    /**
     * @brief 格式化日志记录
     */
    template <LogLevel LEVEL, typename... Args>
    void format(format_string_wrapper<Args...> fmt_w, Args&&... args) {
        if (LEVEL < level_) {
            return;
        }

        std::string message;
        try {
            message = std::format(fmt_w.fmt, std::forward<Args>(args)...);
        } catch (const std::format_error& e) {
#ifndef NDEBUG
            message = std::format(
                "[FORMAT ERROR] {} | fmt=\"{}\"",
                e.what(),
                fmt_w.fmt.get());
#else
            message = fmt_w.fmt.get();
#endif
        }

        static_cast<D*>(this)->template log<LEVEL>(
            LogRecord{
                .datetime = util::DateTime::now(),
                .pid = get_current_pid(),
                .file_name = fmt_w.loc.file_name(),
                .line = fmt_w.loc.line(),
                .log = std::move(message),
            });
    }

private:
    LogLevel level_{LogLevel::Debug};
};

class ConsoleLogger : public BasicLogger<ConsoleLogger> {
public:
    template <LogLevel level>
    void log(const LogRecord& r) {
        LogLevelWrapper lvl(level);

        io::println(std::format(
            "{} [{}{}{}] [pid={}] {}:{} {}",
            r.datetime,
            lvl.color(),
            lvl.__str__(),
            reset_color(),
            r.pid,
            r.file_name,
            r.line,
            r.log));
    }
};

/**
 * @brief 控制台日志器
 */
inline auto& console = Singleton<ConsoleLogger>::instance();

/**
 * @brief 设置控制台日志级别
 */
static auto set_consolelog_level(LogLevel level) {
    console.set_level(level);
}

} // namespace my::log

#endif // LOG_HPP
