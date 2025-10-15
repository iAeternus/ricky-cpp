/**
 * @brief 通用异常基础设施
 * @author Ricky
 * @date 2025/6/20
 * @version 1.0
 */
#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include "Object.hpp"
#include "ricky_format.hpp"

#include <exception>
#include <unordered_map>

namespace my {

//// 前向声明
// void log_exception(const char*, std::source_location);

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
    AssertionFailedException,
    // 系统
    SystemException,
    NetworkException,
    // 自定义
    CustomException
};

/**
 * @class Exception
 * @brief 基础异常类
 * @details 提供异常类型、消息和位置等信息
 */
class Exception final : public std::exception, public Object<Exception> {
public:
    using Self = Exception;

    /**
     * @brief 构造函数
     * @param type 异常类型
     * @param message 异常消息
     * @param loc 异常位置
     * @param nested 关联异常
     */
    explicit Exception(const ExceptionType type, CString&& message,
                       const std::source_location loc = SRC_LOC,
                       std::exception_ptr nested = nullptr) :
            type_(type),
            message_(std::move(message)),
            loc_(loc),
            nested_(std::move(nested)) {
        formatted_message_ = format_message();
    }

    /**
     * @brief 获取异常信息
     */
    [[nodiscard]] const char* what() const noexcept override {
        return formatted_message_.data();
    }

    /**
     * @brief 获取异常类型
     */
    [[nodiscard]] ExceptionType type() const noexcept {
        return type_;
    }

    /**
     * @brief 获取异常类型名称
     */
    [[nodiscard]] CString type_name() const {
        return type_to_string(type_);
    }

    /**
     * @brief 获取异常消息
     */
    [[nodiscard]] CString message() const {
        return message_;
    }

    //    /**
    //     * @brief 记录异常到日志
    //     */
    //    void log() const {
    //        log_exception(what(), loc_);
    //    }

    /**
     * @brief 异常类型转字符串
     */
    static CString type_to_string(const ExceptionType type) {
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
            {ExceptionType::AssertionFailedException, "AssertionFailedException"},
            {ExceptionType::SystemException, "SystemException"},
            {ExceptionType::NetworkException, "NetworkException"},
            {ExceptionType::CustomException, "CustomException"},
        };

        return type_names.contains(type) ? type_names.at(type) : "UnknownException";
    }

    [[nodiscard]] CString __str__() const {
        return formatted_message_;
    }

private:
    [[nodiscard]] CString format_message() const noexcept {
        try {
            auto msg = std::format("{} [{}:{} in {}]: {}",
                                   type_to_string(type_),
                                   loc_.file_name(),
                                   loc_.line(),
                                   loc_.function_name(),
                                   message_);
            // 处理嵌套异常
            if (nested_) {
                try {
                    std::rethrow_exception(nested_);
                } catch (const std::exception& e) {
                    msg += std::format("\n  Caused by: {}", e.what());
                } catch (...) {
                    msg += "\n  Caused by: unknown exception";
                }
            }
            return {msg};
        } catch (const std::bad_alloc&) {
            return "Memory error in exception formatting";
        } catch (...) {
            // 格式化失败时回退
            return {"Exception formatting failed"};
        }
    }

private:
    ExceptionType type_;        // 异常类型
    CString message_;           // 异常消息
    std::source_location loc_;  // 异常发生位置
    std::exception_ptr nested_; // 嵌套异常
    CString formatted_message_; // 格式化后的异常消息
};

///**
// * @brief 使用异常信息字符串创建异常对象
// * @param type 异常类型
// * @param message 异常消息
// * @param loc 异常位置
// * @return 异常对象
// */
// fn exception(const ExceptionType type, CString&& message, const std::source_location loc = SRC_LOC) -> Exception {
//    return Exception(type, std::move(message), loc, nullptr);
//}

/**
 * @brief 使用格式化字符串创建异常对象
 * @param type 异常类型
 * @param fmt 格式化字符串
 * @param loc 异常位置
 * @param args 格式化参数
 * @return 异常对象
 */
template <typename... Args>
fn exception(const ExceptionType type, format_string_wrapper<Args...> fmt_w, Args&&... args) -> Exception {
    return Exception(type, std::format(fmt_w.fmt, std::forward<Args>(args)...), fmt_w.loc);
}

///**
// * @brief 如果条件不满足，则抛出指定类型的异常
// * @param condition 条件
// * @param type 异常类型
// * @param message 异常消息
// * @param loc 异常位置
// */
// fn check(const bool condition, const ExceptionType type, CString&& message, const std::source_location loc = SRC_LOC) -> void {
//    if (!condition) {
//        throw exception(type, std::move(message), loc);
//    }
//}

/**
 * @brief 如果条件不满足，则抛出指定类型的异常
 * @param condition 条件
 * @param type 异常类型
 * @param fmt 格式化字符串
 * @param loc 异常位置
 * @param args 格式化参数
 */
template <typename... Args>
fn check(const bool condition, const ExceptionType type, format_string_wrapper<Args...> fmt_w, Args&&... args) -> void {
    if (!condition) {
        throw exception(type, std::format(fmt_w.fmt, std::forward<Args>(args)...), fmt_w.loc);
    }
}

/**
 * @brief 异常工厂宏，简化异常的创建和使用
 * @note 这些宏会生成对应类型的异常工厂函数
 * @example
 *   auto e = runtime_exception("Something went wrong");
 *   throw e;
 */
#define DEFINE_EXCEPTION_FACTORY(NAME, TYPE)                                                    \
    template <typename... Args>                                                                 \
    fn NAME##_exception(format_string_wrapper<Args...> fmt_w, Args&&... args)->Exception {      \
        return Exception(TYPE, std::format(fmt_w.fmt, std::forward<Args>(args)...), fmt_w.loc); \
    }

DEFINE_EXCEPTION_FACTORY(runtime, ExceptionType::RuntimeException)                      // 运行时异常
DEFINE_EXCEPTION_FACTORY(logic, ExceptionType::LogicException)                          // 逻辑异常
DEFINE_EXCEPTION_FACTORY(value, ExceptionType::ValueException)                          // 数值异常
DEFINE_EXCEPTION_FACTORY(type, ExceptionType::TypeException)                            // 类型异常
DEFINE_EXCEPTION_FACTORY(argument, ExceptionType::ArgumentException)                    // 参数异常
DEFINE_EXCEPTION_FACTORY(null_pointer, ExceptionType::NullPointerException)             // 空指针异常
DEFINE_EXCEPTION_FACTORY(not_found, ExceptionType::NotFoundException)                   // not_found 异常
DEFINE_EXCEPTION_FACTORY(index_out_of_bounds, ExceptionType::IndexOutOfBoundsException) // 下标越界异常
DEFINE_EXCEPTION_FACTORY(resource, ExceptionType::ResourceException)                    // 资源异常
DEFINE_EXCEPTION_FACTORY(io, ExceptionType::IOException)                                // IO 异常
DEFINE_EXCEPTION_FACTORY(memory, ExceptionType::MemoryException)                        // 内存异常
DEFINE_EXCEPTION_FACTORY(arithmetic, ExceptionType::ArithmeticException)                // 算术异常
DEFINE_EXCEPTION_FACTORY(overflow, ExceptionType::OverflowException)                    // 溢出异常
DEFINE_EXCEPTION_FACTORY(state, ExceptionType::StateException)                          // 状态异常
DEFINE_EXCEPTION_FACTORY(uninitialized, ExceptionType::UninitializedException)          // 未初始化异常
DEFINE_EXCEPTION_FACTORY(assertion_failed, ExceptionType::AssertionFailedException)     // 断言失败异常
DEFINE_EXCEPTION_FACTORY(system, ExceptionType::SystemException)                        // 系统异常
DEFINE_EXCEPTION_FACTORY(network, ExceptionType::NetworkException)                      // 网络异常
DEFINE_EXCEPTION_FACTORY(custom, ExceptionType::CustomException)                        // 自定义异常

#undef DEFINE_EXCEPTION_FACTORY // 防止宏污染

} // namespace my

#endif // EXCEPTION_HPP