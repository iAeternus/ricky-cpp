/**
 * @brief
 * @author Ricky
 * @date 2026/2/15
 * @version 1.0
 */
#ifndef OPTION_HPP
#define OPTION_HPP

#include <utility>
#include <type_traits>
#include <concepts>

namespace my {

template <typename T>
class Option {
public:
    // None
    constexpr Option() noexcept :
            has_(false) {}

    // Some
    constexpr Option(const T& v) :
            has_(true), value_(v) {}

    constexpr Option(T&& v) :
            has_(true), value_(std::move(v)) {}

    constexpr Option(const Option& other) :
            has_(other.has_) {
        if (has_) new (&value_) T(other.value_);
    }

    constexpr Option(Option&& other) noexcept :
            has_(other.has_) {
        if (has_) new (&value_) T(std::move(other.value_));
    }

    ~Option() {
        if (has_) value_.~T();
    }

    constexpr Option& operator=(const Option& other) {
        if (this == &other) return *this;
        this->~Option();
        has_ = other.has_;
        if (has_) new (&value_) T(other.value_);
        return *this;
    }

    constexpr Option& operator=(Option&& other) noexcept {
        if (this == &other) return *this;
        this->~Option();
        has_ = other.has_;
        if (has_) new (&value_) T(std::move(other.value_));
        return *this;
    }

    constexpr bool is_some() const noexcept { return has_; }
    constexpr bool is_none() const noexcept { return !has_; }

    constexpr T& unwrap() & { return value_; }
    constexpr const T& unwrap() const& { return value_; }
    constexpr T&& unwrap() && { return std::move(value_); }

    constexpr T& expect([[maybe_unused]] const char* msg) & {
        return value_;
    }

    template <typename U>
    constexpr T unwrap_or(U&& fallback) const {
        return has_ ? value_ : static_cast<T>(std::forward<U>(fallback));
    }

    template <typename F>
        requires std::invocable<F, const T&>
    constexpr auto map(F&& f) const {
        using U = std::invoke_result_t<F, const T&>;
        if (has_) {
            return Option<U>(f(value_));
        } else {
            return Option<U>();
        }
    }

    template <typename F>
        requires std::invocable<F, const T&>
    constexpr auto and_then(F&& f) const {
        using R = std::invoke_result_t<F, const T&>;
        if (has_) {
            return f(value_);
        } else {
            return R();
        }
    }

private:
    bool has_;
    union {
        T value_;
    };
};

} // namespace my

#endif // OPTION_HPP
