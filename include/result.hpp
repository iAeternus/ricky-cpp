#ifndef RESULT_HPP
#define RESULT_HPP

#include <utility>
#include <concepts>
#include <type_traits>

namespace my {

template <typename T, typename E>
class Result {
public:
    static constexpr Result Ok(const T& v) {
        Result r;
        r.ok_ = true;
        new (&r.value_) T(v);
        return r;
    }

    static constexpr Result Ok(T&& v) {
        Result r;
        r.ok_ = true;
        new (&r.value_) T(std::move(v));
        return r;
    }

    static constexpr Result Err(const E& e) {
        Result r;
        r.ok_ = false;
        new (&r.error_) E(e);
        return r;
    }

    static constexpr Result Err(E&& e) {
        Result r;
        r.ok_ = false;
        new (&r.error_) E(std::move(e));
        return r;
    }

    Result(const Result& other) :
            ok_(other.ok_) {
        if (ok_) {
            new (&value_) T(other.value_);
        } else {
            new (&error_) E(other.error_);
        }
    }

    Result(Result&& other) noexcept :
            ok_(other.ok_) {
        if (ok_) {
            new (&value_) T(std::move(other.value_));
        } else {
            new (&error_) E(std::move(other.error_));
        }
    }

    ~Result() {
        if (ok_) {
            value_.~T();
        } else {
            error_.~E();
        }
    }

    constexpr Result& operator=(const Result& other) {
        if (this == &other) return *this;
        this->~Result();
        ok_ = other.ok_;
        if (ok_) {
            new (&value_) T(other.value_);
        } else {
            new (&error_) E(other.error_);
        }
        return *this;
    }

    constexpr Result& operator=(Result&& other) noexcept {
        if (this == &other) return *this;
        this->~Result();
        ok_ = other.ok_;
        if (ok_) {
            new (&value_) T(std::move(other.value_));
        } else {
            new (&error_) E(std::move(other.error_));
        }
        return *this;
    }

    constexpr bool is_ok() const noexcept { return ok_; }
    constexpr bool is_err() const noexcept { return !ok_; }

    constexpr T& unwrap() & { return value_; }
    constexpr const T& unwrap() const& { return value_; }
    constexpr T&& unwrap() && { return std::move(value_); }

    constexpr E& unwrap_err() & { return error_; }
    constexpr const E& unwrap_err() const& { return error_; }
    constexpr E&& unwrap_err() && { return std::move(error_); }

    constexpr T& expect([[maybe_unused]] const char* msg) & {
        return value_;
    }

    constexpr E& expect_err([[maybe_unused]] const char* msg) & {
        return error_;
    }

    template <typename F>
        requires std::invocable<F, const T&>
    constexpr auto map(F&& f) const {
        using U = std::invoke_result_t<F, const T&>;
        if (ok_) {
            return Result<U, E>::Ok(f(value_));
        } else {
            return Result<U, E>::Err(error_);
        }
    }

    template <typename F>
        requires std::invocable<F, const E&>
    constexpr auto map_err(F&& f) const {
        using G = std::invoke_result_t<F, const E&>;
        if (!ok_) {
            return Result<T, G>::Err(f(error_));
        } else {
            return Result<T, G>::Ok(value_);
        }
    }

    template <typename F>
        requires std::invocable<F, const T&>
    constexpr auto and_then(F&& f) const {
        using R = std::invoke_result_t<F, const T&>;
        if (ok_) {
            return f(value_);
        } else {
            return R::Err(error_);
        }
    }

    template <typename U>
    constexpr T unwrap_or(U&& fallback) const {
        return ok_ ? value_ : static_cast<T>(std::forward<U>(fallback));
    }

private:
    constexpr Result() :
            ok_(false) {}

private:
    bool ok_;
    union {
        T value_;
        E error_;
    };
};

} // namespace my

#endif // RESULT_HPP
