#ifndef MATCH_HPP
#define MATCH_HPP

#include "ricky.hpp"

#include <variant>

namespace my {

template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts... ts) -> overloaded<Ts...>;

template <typename... T, typename... F>
fn match(const std::variant<T...>& value, F&&... funcs) {
    return std::visit(overloaded(std::forward<F>(funcs)...), value);
}

} // namespace my

#endif // MATCH_HPP