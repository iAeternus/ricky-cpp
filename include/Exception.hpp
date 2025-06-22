/**
 * @brief 通用异常基础设施
 * @author Ricky
 * @date 2025/6/20
 * @version 1.0
 */
#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include "Object.hpp"

#include <exception>
#include <source_location>
#include <format>
#include <unordered_map>

// 前向声明
namespace my::io {

void log_exception(const char* msg);

} // namespace my::io

namespace my {

/**
 * @brief 异常类型枚举
 */
enum class ExceptionType {
    // 核心异常
    RuntimeException,
    LogicException,
    // 参数
    ValueException,
    TypeException,
    ArgumentException,
    NullPointerException,
    NotFoundException,
    IndexOutOfBoundsException,
    // 资源
    ResourceException,
    IOException,
    MemoryException,
    // 计算
    ArithmeticException,
    OverflowException,
    // 状态
    StateException,
    UninitializedException,
    // 系统
    SystemException,
    NetworkException,
    // 自定义
    CustomException
};

/**
 * @brief 通用异常类
 */
class Exception : public std::exception, public Object<Exception> {
public:
    using Self = Exception;

    /**
     * @brief 构造函数
     * @param type 异常类型
     * @param message 异常消息
     * @param loc 异常位置
     */
    Exception(ExceptionType type, CString&& message, std::source_location loc = std::source_location::current()) :
            type_(type), message_(std::move(message)), loc_(loc), formatted_message_(format_message()) {}

    /**
     * @brief 获取异常信息
     */
    const char* what() const noexcept override {
        return formatted_message_.data();
    }

    /**
     * @brief 获取异常类型
     */
    ExceptionType type() const noexcept {
        return type_;
    }

    /**
     * @brief 获取异常类型名称
     */
    CString type_name() const {
        return type_to_string(type_);
    }

    /**
     * @brief 获取异常消息
     */
    CString message() const {
        return message_;
    }

    /**
     * @brief 记录异常到日志
     */
    void log() const {
        io::log_exception(what());
    }

    /**
     * @brief 异常类型转字符串
     */
    static CString type_to_string(ExceptionType type) {
        static const std::unordered_map<ExceptionType, CString> type_names = {
            {ExceptionType::RuntimeException, "RuntimeException"},
            {ExceptionType::LogicException, "LogicException"},
            {ExceptionType::ValueException, "ValueException"},
            {ExceptionType::TypeException, "TypeException"},
            {ExceptionType::ArgumentException, "ArgumentException"},
            {ExceptionType::NullPointerException, "NullPointerException"},
            {ExceptionType::NotFoundException, "NotFoundException"},
            {ExceptionType::IndexOutOfBoundsException, "IndexOutOfBoundsException"},
            {ExceptionType::ResourceException, "ResourceException"},
            {ExceptionType::IOException, "IOException"},
            {ExceptionType::MemoryException, "MemoryException"},
            {ExceptionType::ArithmeticException, "ArithmeticException"},
            {ExceptionType::OverflowException, "OverflowException"},
            {ExceptionType::StateException, "StateException"},
            {ExceptionType::UninitializedException, "UninitializedException"},
            {ExceptionType::SystemException, "SystemException"},
            {ExceptionType::NetworkException, "NetworkException"},
            {ExceptionType::CustomException, "CustomException"}};

        return type_names.contains(type) ? type_names.at(type) : "UnknownException";
    }

    CString __str__() const {
        return formatted_message_;
    }

private:
    CString format_message() const {
        try {
            return std::format("{} [{}:{} in {}]: {}",
                               type_to_string(type_),
                               loc_.file_name(),
                               loc_.line(),
                               loc_.function_name(),
                               message_);
        } catch (...) {
            // 格式失败时回退
            return CString("Exception formatting failed");
        }
    }

private:
    ExceptionType type_;
    CString message_;
    std::source_location loc_;
    CString formatted_message_;
};

/**
 * @brief 基本异常工厂
 */
fn exception(ExceptionType type, CString&& message, std::source_location loc = std::source_location::current()) {
    throw Exception(type, std::move(message), loc);
}

template <typename... Args>
fn exception(ExceptionType type, std::string_view fmt, std::source_location loc, Args&&... args) {
    std::string message = std::vformat(fmt, std::make_format_args(args...));
    throw Exception(type, std::move(message), loc);
}

/**
 * @brief 条件检查
 */
fn check(bool condition, ExceptionType type, CString&& message, std::source_location loc = std::source_location::current()) {
    if (!condition) {
        exception(type, std::move(message), loc);
    }
}

template <typename... Args>
fn check(bool condition, ExceptionType type, std::string_view fmt, std::source_location loc, Args&&... args) {
    if (!condition) {
        std::string message = std::vformat(fmt, std::make_format_args(args...));
        exception(type, std::move(message), loc);
    }
}

/**
 * @brief 异常工厂
 */
#define DEFINE_EXCEPTION_FACTORY(NAME, TYPE)                                     \
    fn NAME##_exception(                                                         \
        CString&& message,                                                       \
        std::source_location loc = std::source_location::current()) {            \
        exception(TYPE, std::move(message), loc);                                \
    }                                                                            \
    template <typename... Args>                                                  \
    fn NAME##_exception(                                                         \
        std::string_view fmt,                                                    \
        std::source_location loc,                                                \
        Args&&... args) {                                                        \
        std::string message = std::vformat(fmt, std::make_format_args(args...)); \
        exception(TYPE, std::move(message), loc);                                \
    }

DEFINE_EXCEPTION_FACTORY(runtime, ExceptionType::RuntimeException)                      // 运行时异常
DEFINE_EXCEPTION_FACTORY(logic, ExceptionType::LogicException)                          // 逻辑异常
DEFINE_EXCEPTION_FACTORY(value, ExceptionType::ValueException)                          // 数值异常
DEFINE_EXCEPTION_FACTORY(type, ExceptionType::TypeException)                            // 类型异常
DEFINE_EXCEPTION_FACTORY(argument, ExceptionType::ArgumentException)                    // 参数异常
DEFINE_EXCEPTION_FACTORY(null_pointer, ExceptionType::NullPointerException)             // 空指针异常
DEFINE_EXCEPTION_FACTORY(not_found, ExceptionType::NotFoundException)                   // not_found异常
DEFINE_EXCEPTION_FACTORY(index_out_of_bounds, ExceptionType::IndexOutOfBoundsException) // 下标越界异常
DEFINE_EXCEPTION_FACTORY(resource, ExceptionType::ResourceException)                    // 资源异常
DEFINE_EXCEPTION_FACTORY(io, ExceptionType::IOException)                                // IO异常
DEFINE_EXCEPTION_FACTORY(memory, ExceptionType::MemoryException)                        // 内存异常
DEFINE_EXCEPTION_FACTORY(arithmetic, ExceptionType::ArithmeticException)                // 算术异常
DEFINE_EXCEPTION_FACTORY(overflow, ExceptionType::OverflowException)                    // 溢出异常
DEFINE_EXCEPTION_FACTORY(state, ExceptionType::StateException)                          // 状态异常
DEFINE_EXCEPTION_FACTORY(uninitialized, ExceptionType::UninitializedException)          // 未初始化异常
DEFINE_EXCEPTION_FACTORY(system, ExceptionType::SystemException)                        // 系统异常
DEFINE_EXCEPTION_FACTORY(network, ExceptionType::NetworkException)                      // 网络异常
DEFINE_EXCEPTION_FACTORY(custom, ExceptionType::CustomException)                        // 自定义异常

#undef DEFINE_EXCEPTION_FACTORY // 防止宏污染

#define SRC_LOC std::source_location::current()

} // namespace my

#endif // EXCEPTION_HPP