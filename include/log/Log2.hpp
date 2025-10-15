/**
 * @brief 日志
 * @author Ricky
 * @date 2025/10/1
 * @version 1.0
 */

#ifndef LOG2_HPP
#define LOG2_HPP

// #include "String.hpp"
#include "Exception.hpp"
#include "Color.hpp"
#include "NoCopy.hpp"
#include "Printer.hpp"

#include <chrono>

#ifdef _WIN32
#include "processthreadsapi.h"
#else
#include <sys/types.h>
#include <unistd.h>
#endif // _WIN32

/**
 * TODO 优化原有的日志功能，支持格式化/文件输出/线程安全
 *
 * # Examples
 * // 预定义文件日志路径
 * add_definitions(-DFILE_LOG1_PATH="${CMAKE_CURRENT_SOURCE_DIR}/logs/log1/")
 *
 * // 打印控制台日志
 * log::set_consolelog_level(fastlog::LogLevel::Trace);
 * log::console.trace("hello world");
 * log::console.debug("hello world");
 * log::console.info("hello world");
 * log::console.warn("hello world");
 * log::console.error("hello world");
 * log::console.fatal("hello world");
 *
 * // 输出文件日志
 * log::file::get_logger("file_log1").info("hello world log1,count : {}, vec :{}", count++, vec);
 * log::file::get_logger("file_log2").info("hello world log1,count : {}, vec :{}", count++, vec);
 *
 * // 注册文件日志器
 * auto &file_logger1 = log::file::make_logger("file_log1", FILE_LOG1_PATH);
 */

namespace my::log {

/**
 * @brief 日志级别
 */
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
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
        case LogLevel::TRACE: return color::Color::CYAN;
        case LogLevel::DEBUG: return color::Color::BLUE;
        case LogLevel::INFO: return color::Color::GREEN;
        case LogLevel::WARN: return color::Color::YELLOW;
        case LogLevel::ERROR: return color::Color::RED;
        case LogLevel::FATAL: return color::Color::PURPLE;
        default: throw argument_exception("Unknown log level");
        }
    }

    CString __str__() const {
        switch (level_) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: throw argument_exception("Unknown log level");
        }
    }

private:
    LogLevel level_;
};

/**
 * @brief 重置控制台颜色
 */
fn reset_color() noexcept -> const char* {
    return color::Color::CLOSE;
}

/**
 * @brief 日志记录
 */
struct LogRecord {
    const char* datetime;  // 日志记录时间
    u32 pid;               // 进程ID
    const char* file_name; // 文件名
    usize line;            // 行号
    std::string log;       // 日志内容
};

/**
 * @brief 获取当前时间并转为string
 * @param is_repeat 是否重复获取
 */
std::optional<std::string> get_current_time_tostring(bool is_repeat = true) {
    static thread_local std::array<char, 64> buf{};
    static thread_local std::chrono::seconds last_second{};

    // 获取当前时间
    const auto now = std::chrono::system_clock::now();
    // 转换为time_t类型
    const auto time_t_now = std::chrono::system_clock::to_time_t(now);
    // 转换为秒
    const auto current_second = std::chrono::seconds(time_t_now);

    // 检查是否是新的秒
    if ((current_second.count() != last_second.count()) || is_repeat) {
        // 转换为本地时间
        std::tm* local_tm = std::localtime(&time_t_now);

        // 根据平台使用不同的时间格式
#ifdef _WIN32
        // Windows平台：使用连字符替代冒号，避免文件名非法字符
        std::strftime(buf.data(), buf.size(), "%Y-%m-%d-%H-%M-%S", local_tm);
#else
        // Unix/Linux/macOS平台：可以使用冒号 TODO include linux头文件
        std::strftime(buf.data(), buf.size(), "%Y-%m-%d-%H:%M:%S", local_tm);
#endif

        last_second = current_second;
        return {buf.data()};
    }
    return std::nullopt;
}

/**
 * @brief 获取当前pid
 */
inline auto get_current_pid() -> u32 {
#ifdef _WIN32
    return static_cast<u32>(GetCurrentProcessId());
#else
    return static_cast<u32>(getpid());
#endif
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
    void trace(format_string_wrapper<Args...> fmt, Args&&... args) {
        format<LogLevel::TRACE>(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(format_string_wrapper<Args...> fmt, Args&&... args) {
        format<LogLevel::DEBUG, Args...>(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(format_string_wrapper<Args...> fmt, Args&&... args) {
        format<LogLevel::INFO>(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(format_string_wrapper<Args...> fmt, Args&&... args) {
        format<LogLevel::WARN, Args...>(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(format_string_wrapper<Args...> fmt, Args&&... args) {
        format<LogLevel::ERROR, Args...>(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void fatal(format_string_wrapper<Args...> fmt, Args&&... args) {
        format<LogLevel::FATAL>(fmt, std::forward<Args>(args)...);
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
        std::string time_str;
        auto res = get_current_time_tostring();
        if (res.has_value()) {
            time_str = res.value();
        }
        // 调用派生类的log方法记录日志
        static_cast<D*>(this)->template log<LEVEL>(
            LogRecord{.datetime = time_str.c_str(),
                      .pid = get_current_pid(),
                      .file_name = fmt_w.loc.file_name(),
                      .line = fmt_w.loc.line(),
                      .log = std::format(fmt_w.fmt, std::forward<Args>(args)...)});
    }

private:
    LogLevel level_{LogLevel::DEBUG};
};

class ConsoleLogger : public BasicLogger<ConsoleLogger> {
public:
    template <LogLevel level>
    void log(const LogRecord& record) {
        LogLevelWrapper level_wrapper(level);
        io::println(std::format("{} [{}{}{}] {} {}:{} {}", record.datetime, level_wrapper.color(),
                                level_wrapper.__str__(), reset_color(), record.pid, record.file_name, record.line,
                                record.log));
    }
};

/**
 * @brief 控制台日志器
 */
inline auto& console = Singleton<ConsoleLogger>::instance();

/**
 * @brief 设置控制台日志级别
 */
static fn set_consolelog_level(LogLevel level) {
    console.set_level(level);
}

} // namespace my::log

#endif // LOG2_HPP
