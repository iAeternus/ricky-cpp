// /**
//  * @brief 收集器工厂，支持复杂收集操作和并行处理
//  * @author Ricky
//  * @date 2025/1/31
//  * @version 2.0
//  */
// #ifndef COLLECTORS_HPP
// #define COLLECTORS_HPP

// #include "ricky_concepts.hpp"
// #include "DynArray.hpp"
// #include "Dict.hpp"
// #include "Generator.hpp"

// namespace my::util {

// //---------------------- 收集器概念 ----------------------
// template <typename C, typename T>
// concept Collector = requires(C c, coro::Generator<T> gen, const DynArray<T>& arr) {
//     { c.collect(gen) }
//     ->std::convertible_to<typename C::result_type>;
//     { c.parallel_collect(arr) }
//     ->std::convertible_to<typename C::result_type>;
// };

// //---------------------- 基础收集器 ----------------------
// template <typename Container>
// struct ToCollector {
//     using result_type = Container;

//     template <typename T>
//     Container collect(coro::Generator<T> gen) const {
//         Container container;
//         for (const T& elem : gen) {
//             if constexpr (requires { container.push_back(elem); }) {
//                 container.push_back(elem);
//             } else if constexpr (requires { container.insert(elem); }) {
//                 container.insert(elem);
//             } else if constexpr (requires { container.append(elem); }) {
//                 container.append(elem);
//             } else {
//                 ValueError("Unsupported container type");
//                 return None<Container>;
//             }
//         }
//         return container;
//     }

//     template <typename T>
//     Container parallel_collect(const DynArray<T>& buffer) const {
//         Container container;
//         for (const auto& elem : buffer) {
//             if constexpr (requires { container.push_back(elem); }) {
//                 container.push_back(elem);
//             } else {
//                 static_assert("Parallel collection not supported for this container");
//             }
//         }
//         return container;
//     }
// };

// template <typename T>
// struct ToDynArrayCollector {
//     using result_type = DynArray<T>;

//     DynArray<T> collect(coro::Generator<T> gen) const {
//         DynArray<T> arr;
//         for (const T& elem : gen) {
//             arr.append(elem);
//         }
//         return arr;
//     }

//     DynArray<T> parallel_collect(const DynArray<T>& buffer) const {
//         return buffer; // 直接返回已缓冲数据
//     }
// };

// //---------------------- 分组收集器 ----------------------
// template <typename KeyMapper, typename ValueMapper>
// struct GroupByCollector {
//     using KeyType = std::decay_t<std::invoke_result_t<KeyMapper, typename std::invoke_result_t<ValueMapper>::argument_type>>;
//     using ValueType = std::decay_t<std::invoke_result_t<ValueMapper, typename std::invoke_result_t<ValueMapper>::argument_type>>;
//     using result_type = Dict<KeyType, DynArray<ValueType>>;

//     KeyMapper key_mapper;
//     ValueMapper value_mapper;

//     template <typename T>
//     result_type collect(coro::Generator<T> gen) const {
//         result_type result;
//         for (const T& elem : gen) {
//             auto&& key = key_mapper(elem);
//             auto&& value = value_mapper(elem);
//             result[key].append(std::forward<decltype(value)>(value));
//         }
//         return result;
//     }

//     template <typename T>
//     result_type parallel_collect(const DynArray<T>& buffer) const {
//         // 简化实现：串行处理缓冲数据
//         result_type result;
//         for (const auto& elem : buffer) {
//             auto&& key = key_mapper(elem);
//             auto&& value = value_mapper(elem);
//             result[key].append(std::forward<decltype(value)>(value));
//         }
//         return result;
//     }
// };

// //---------------------- 收集器工厂 ----------------------
// class Collectors {
// public:
//     template <typename Container>
//     static ToCollector<Container> to() noexcept {
//         return {};
//     }

//     template <typename T>
//     static ToDynArrayCollector<T> toDynArray() noexcept {
//         return {};
//     }

//     template <typename KeyMapper, typename ValueMapper>
//     static auto groupBy(KeyMapper&& keyMapper, ValueMapper&& valueMapper) {
//         return GroupByCollector<std::decay_t<KeyMapper>, std::decay_t<ValueMapper>>{
//             std::forward<KeyMapper>(keyMapper),
//             std::forward<ValueMapper>(valueMapper)};
//     }
// };

// } // namespace my::util

// #endif // COLLECTORS_HPP