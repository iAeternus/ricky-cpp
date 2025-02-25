/**
 * @brief 轻量级日志
 * @author Ricky
 * @date 2025/2/23
 * @version 1.0
 */
#ifndef LOG_HPP
#define LOG_HPP

#include "DateTime.hpp"
#include "DynArray.hpp"

#include <mutex>

namespace my::io {

class Log : public Object<Log> {
    using self = Log;

public:
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

        static constexpr const char* LEVEL_NAMES[] = {
            "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

        static constexpr const char* LEVEL_COLORS[] = {
            Color::AQUA, Color::DEEPGREEN, Color::GREEN, Color::YELLOW, Color::RED, Color::PURPLE};
    };

    /**
     * @brief 日志处理器
     * @note 可以支持日志等级与输出文件间多对多的关系
     */
    class LogHandler : public Object<LogHandler> {
    public:
        constexpr LogHandler(i32 level = LogLevel::INACTIVE_, FILE* output = nullptr, bool enable_color = true) :
                level_(level), output_(output), enableColor_(enable_color) {}

        i32 level_;        // 日志等级
        FILE* output_;     // 指向输出文件的指针
        bool enableColor_; // 是否允许颜色打印
    };

    /**
     * @brief 添加日志配置
     */
    static void addHandler(const LogHandler& handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        try {
            handlers_.append(handler);
            if (handler.level_ < minLevel_) {
                minLevel_ = handler.level_;
            }
        } catch (...) {
            fprintf(stderr, "[ERROR] Failed to add log handler\n");
        }
    }

    /**
     * @brief 设置最低日志级别
     */
    static void setLevel(i32 level) {
        std::lock_guard<std::mutex> lock(mutex_);
        minLevel_ = level;
    }

    template <ConvertibleToCstr Str>
    static void trace(const Str& msg, std::source_location loc = std::source_location::current()) {
        log_impl(msg, LogLevel::TRACE_, loc);
    }

    template <ConvertibleToCstr Str>
    static void debug(const Str& msg, std::source_location loc = std::source_location::current()) {
        log_impl(msg, LogLevel::DEBUG_, loc);
    }

    template <ConvertibleToCstr Str>
    static void info(const Str& msg, std::source_location loc = std::source_location::current()) {
        log_impl(msg, LogLevel::INFO_, loc);
    }

    template <ConvertibleToCstr Str>
    static void warn(const Str& msg, std::source_location loc = std::source_location::current()) {
        log_impl(msg, LogLevel::WARN_, loc);
    }

    template <ConvertibleToCstr Str>
    static void error(const Str& msg, std::source_location loc = std::source_location::current()) {
        log_impl(msg, LogLevel::ERROR_, loc);
    }

    template <ConvertibleToCstr Str>
    static void fatal(const Str& msg, std::source_location loc = std::source_location::current()) {
        log_impl(msg, LogLevel::FATAL_, loc);
    }

private:
    static std::mutex mutex_;
    static util::DynArray<LogHandler> handlers_;
    static i32 minLevel_;

    template <ConvertibleToCstr Str>
    static void log_impl(const Str msg, i32 level, std::source_location loc = std::source_location::current(), util::DateTime dateTime = util::DateTime::now()) {
        if (level < minLevel_) return;

        // 获取当前配置快照
        util::DynArray<LogHandler> localHandlers;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            localHandlers = handlers_;
        }

        // 通知所有级别等于给定级别的处理器打印日志
        for (const auto& handler : localHandlers) {
            if (handler.level_ == level) {
                print_LogHandler(handler, msg, dateTime, get_basename(loc.file_name()), loc.line());
            }
        }
    }

    static const char* get_basename(const char* path) {
        const char* slash = strrchr(path, '/');
        if (!slash) slash = strrchr(path, '\\');
        return slash ? slash + 1 : path;
    }

    static void print_LogHandler(const LogHandler& evt, const char* msg, const util::DateTime& dateTime, const char* fileName, i32 line) {
        fprintf(evt.output_, "%s %s%-5s%s %s:%d ",
                dateTime.__str__().data(),
                evt.enableColor_ ? LogLevel::LEVEL_COLORS[evt.level_] : Color::CLOSE,
                LogLevel::LEVEL_NAMES[evt.level_],
                Color::CLOSE,
                fileName, line);
        fprintf(evt.output_, msg);
        fprintf(evt.output_, "\n");
        fflush(evt.output_);
    }
};

std::mutex Log::mutex_;
util::DynArray<Log::LogHandler> Log::handlers_{
    Log::LogHandler(Log::LogLevel::TRACE_, stdout),
    Log::LogHandler(Log::LogLevel::DEBUG_, stdout),
    Log::LogHandler(Log::LogLevel::INFO_, stdout),
    Log::LogHandler(Log::LogLevel::WARN_, stdout),
    Log::LogHandler(Log::LogLevel::ERROR_, stdout),
    Log::LogHandler(Log::LogLevel::FATAL_, stdout),
};
i32 Log::minLevel_ = Log::LogLevel::INFO_;

} // namespace my::io

#endif // LOG_HPP