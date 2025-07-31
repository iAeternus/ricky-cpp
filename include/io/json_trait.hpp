/**
 * @brief Json类型萃取
 * @author Ricky
 * @date 2025/1/11
 * @version 1.0
 */
#ifndef JSON_TRAIT_HPP
#define JSON_TRAIT_HPP

#include "String.hpp"
#include "Vec.hpp"
#include "Dict.hpp"

namespace my::io {

class Json;

class Null {};

/**
 * @brief Json类型枚举
 */
struct JsonType {
    using JsonInt = i64;
    using JsonFloat = f64;
    using JsonBool = bool;
    using JsonStr = util::String;
    using JsonArray = util::Vec<Json>;
    using JsonDict = util::Dict<JsonStr, Json>;
    using JsonNull = Null;
};

/**
 * @brief 严格类型约束
 */
template <typename T>
concept JsonTypeStrictConcept =
    std::is_same_v<T, typename JsonType::JsonInt> || std::is_same_v<T, typename JsonType::JsonFloat> || std::is_same_v<T, typename JsonType::JsonBool> || std::is_same_v<T, typename JsonType::JsonStr> || std::is_same_v<T, typename JsonType::JsonArray> || std::is_same_v<T, typename JsonType::JsonDict> || std::is_same_v<T, typename JsonType::JsonNull> || std::is_same_v<T, Json>;

/**
 * @brief 可接受const reference volatile 修饰符的概念
 */
template <typename T>
concept JsonTypeConcept = JsonTypeStrictConcept<std::remove_cvref_t<T>>;

/**
 * @brief 严格类型获得Json类型的枚举值
 */
template <JsonTypeStrictConcept T>
struct GetJsonTypeIDStrict {
    constexpr static i8 ID = -1;
};

template <>
struct GetJsonTypeIDStrict<typename JsonType::JsonInt> {
    constexpr static i8 ID = 0;
};

template <>
struct GetJsonTypeIDStrict<typename JsonType::JsonFloat> {
    constexpr static i8 ID = 1;
};

template <>
struct GetJsonTypeIDStrict<typename JsonType::JsonBool> {
    constexpr static i8 ID = 2;
};

template <>
struct GetJsonTypeIDStrict<typename JsonType::JsonStr> {
    constexpr static i8 ID = 3;
};

template <>
struct GetJsonTypeIDStrict<typename JsonType::JsonArray> {
    constexpr static i8 ID = 4;
};

template <>
struct GetJsonTypeIDStrict<typename JsonType::JsonDict> {
    constexpr static i8 ID = 5;
};

template <>
struct GetJsonTypeIDStrict<typename JsonType::JsonNull> {
    constexpr static i8 ID = 6;
};

template <>
struct GetJsonTypeIDStrict<Json> {
    constexpr static i8 ID = 7;
};

// // 整数类型支持
// template <typename T>
// struct GetJsonTypeIDStrict<T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>> {
//     static constexpr JsonTypeID ID = JsonTypeID::Int;
// };

// // 浮点类型支持
// template <typename T>
// struct GetJsonTypeIDStrict<T, std::enable_if_t<std::is_floating_point_v<T>>> {
//     static constexpr JsonTypeID ID = JsonTypeID::Float;
// };

// // 布尔类型支持
// template <>
// struct GetJsonTypeIDStrict<bool> {
//     static constexpr JsonTypeID ID = JsonTypeID::Bool;
// };

/**
 * @brief 可接受const reference volatile 修饰符获得Json类型的枚举值
 */
template <JsonTypeConcept T>
struct GetJsonTypeID {
    constexpr static i8 ID = GetJsonTypeIDStrict<std::remove_cvref_t<T>>::ID;
};

/**
 * @brief 类型转换萃取
 */
template <JsonTypeStrictConcept T>
struct JsonTypeTrait;

template <>
struct JsonTypeTrait<typename JsonType::JsonInt> {
    using Type = typename JsonType::JsonInt;
    static constexpr const char* name = "JsonInt";
};

template <>
struct JsonTypeTrait<typename JsonType::JsonFloat> {
    using Type = typename JsonType::JsonFloat;
    static constexpr const char* name = "JsonFloat";
};

template <>
struct JsonTypeTrait<typename JsonType::JsonBool> {
    using Type = typename JsonType::JsonBool;
    static constexpr const char* name = "JsonBool";
};

template <>
struct JsonTypeTrait<typename JsonType::JsonStr> {
    using Type = typename JsonType::JsonStr;
    static constexpr const char* name = "JsonStr";
};

template <>
struct JsonTypeTrait<typename JsonType::JsonArray> {
    using Type = typename JsonType::JsonArray;
    static constexpr const char* name = "JsonArray";
};

template <>
struct JsonTypeTrait<typename JsonType::JsonDict> {
    using Type = typename JsonType::JsonDict;
    static constexpr const char* name = "JsonDict";
};

template <>
struct JsonTypeTrait<typename JsonType::JsonNull> {
    using Type = typename JsonType::JsonNull;
    static constexpr const char* name = "JsonNull";
};

template <>
struct JsonTypeTrait<Json> {
    using Type = Json;
    static constexpr const char* name = "Json";
};

/**
 * @brief 类型转换映射
 */
template <typename T, typename = void>
struct JsonValueType {
    using Type = T; // 默认保留原始类型
};

// 整数类型映射到JsonInt
template <typename T>
struct JsonValueType<T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>> {
    using Type = JsonType::JsonInt;
};

// 浮点类型映射到JsonFloat
template <typename T>
struct JsonValueType<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    using Type = JsonType::JsonFloat;
};

// 布尔类型映射
template <>
struct JsonValueType<bool> {
    using Type = JsonType::JsonBool;
};

// 字符串类型映射
template <>
struct JsonValueType<util::String> {
    using Type = JsonType::JsonStr;
};

// Vec类型映射
template <typename T>
struct JsonValueType<util::Vec<T>> {
    using Type = JsonType::JsonArray;
};

// Dict类型映射
template <typename K, typename V>
struct JsonValueType<util::Dict<K, V>> {
    using Type = std::conditional_t<
        std::is_convertible_v<K, util::String>,
        JsonType::JsonDict,
        void
    >;
    static_assert(
        !std::is_same_v<Type, void>,
        "JsonDict key must be convertible to util::String"
    );
};

// Json类型映射
template <>
struct JsonValueType<Json> {
    using Type = Json;
};

} // namespace my::io

#endif // JSON_TRAIT_HPP