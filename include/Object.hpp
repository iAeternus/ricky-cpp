/**
 * @brief 顶层父类
 * @author Ricky
 * @date 2024/11/24
 * @version 1.0
 */
#ifndef OBJECT_HPP
#define OBJECT_HPP

namespace ricky {

template <typename Derived>
class Object {
public:
    using RickyObjectDerived = Derived;
};

} // namespace ricky

#endif // OBJECT_HPP