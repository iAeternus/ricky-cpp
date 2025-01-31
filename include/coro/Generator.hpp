/**
 * @brief 生成器
 * @author Ricky
 * @date 2025/1/31
 * @version 1.0
 */
#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "Promise.hpp"

#include <exception>
#include <iterator>
#include <utility>

namespace my::coro {

template <typename T>
class Generator : public Object<Generator<T>>, public NoCopy {
    using self = Generator<T>;
    static_assert(std::is_default_constructible_v<T>, "Generator requires default constructible result type");

public:
    struct promise_type;
    using co_handle = std::coroutine_handle<promise_type>;

    struct promise_type : public PromiseImpl<T> {
        self get_return_object() noexcept {
            return self(co_handle::from_promise(*this));
        }

        void unhandled_exception() noexcept {
            exception_ = std::current_exception();
        }

        std::exception_ptr exception_;
    };

    explicit Generator(co_handle handle) noexcept :
            handle_(handle) {}

    Generator(self&& other) noexcept :
            handle_(std::exchange(other.handle_, nullptr)) {}

    self& operator=(self&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    ~Generator() {
        if (handle_) handle_.destroy();
    }

    class Iterator {
    public:
        using self = Iterator;
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = const T&;
        using pointer = const T*;

        explicit Iterator(co_handle handle = nullptr) :
                handle_(handle) {}

        self& operator++() {
            if (!valid_handle()) return *this;

            handle_.resume();
            if (handle_.done()) {
                check_exception();
                handle_ = nullptr;
            }
            return *this;
        }

        self operator++(int) {
            self tmp(*this);
            ++tmp;
            return tmp;
        }

        reference operator*() const noexcept {
            return handle_.promise().value_;
        }

        bool operator==(const self& rhs) const noexcept {
            return handle_ == rhs.handle_;
        }

    private:
        bool valid_handle() const noexcept {
            return handle_ && !handle_.done();
        }

        void check_exception() {
            if (auto& exc = handle_.promise().exception_) {
                std::rethrow_exception(exc);
            }
        }

        co_handle handle_;
    };

    Iterator begin() {
        if (!handle_ || handle_.done()) return end();

        handle_.resume();
        if (handle_.done()) {
            if (auto& exc = handle_.promise().exception_) {
                std::rethrow_exception(exc);
            }
            return end();
        }
        return Iterator(handle_);
    }

    Iterator end() const noexcept {
        return Iterator(nullptr);
    }

private:
    co_handle handle_;
};

} // namespace my::coro

#endif // GENERATOR_HPP