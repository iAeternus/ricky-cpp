/**
 * @brief 生成器
 * @author Ricky
 * @date 2025/1/31
 * @version 1.0
 */
#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "Printer.hpp"
#include "NoCopy.hpp"

namespace my::coro {

template<typename T>
class Generator : public Object<Generator<T>> {
    using self = Generator<T>;
    static_assert(std::is_default_constructible_v<T>, "Generator requires default constructible result type");

public:
    

private:

};

} // namespace my::coro

#endif // GENERATOR_HPP