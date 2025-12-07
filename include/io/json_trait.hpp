/**
 * @brief Json类型萃取
 * @author Ricky
 * @date 2025/1/11
 * @version 1.0
 */
#ifndef JSON_TRAIT_HPP
#define JSON_TRAIT_HPP

#include "str.hpp"
#include "vec.hpp"
#include "hash_map.hpp"

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
    using JsonMap = util::HashMap<JsonStr, Json>;
    using JsonNull = Null;
};

/**
 * @brief 严格类型约束
 */
template <typename T>
concept JsonTypeStrictConcept =
    std::is_same_v<T, typename JsonType::JsonInt> || std::is_same_v<T, typename JsonType::JsonFloat> || std::is_same_v<T, typename JsonType::JsonBool> || std::is_same_v<T, typename JsonType::JsonStr> || std::is_same_v<T, typename JsonType::JsonArray> || std::is_same_v<T, typename JsonType::JsonMap> || std::is_same_v<T, typename JsonType::JsonNull> || std::is_same_v<T, Json>;

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
struct GetJsonTypeIDStrict<JsonType::JsonInt> {
    constexpr static i8 ID = 0;
};

template <>
struct GetJsonTypeIDStrict<JsonType::JsonFloat> {
    constexpr static i8 ID = 1;
};

template <>
struct GetJsonTypeIDStrict<JsonType::JsonBool> {
    constexpr static i8 ID = 2;
};

template <>
struct GetJsonTypeIDStrict<JsonType::JsonStr> {
    constexpr static i8 ID = 3;
};

template <>
struct GetJsonTypeIDStrict<JsonType::JsonArray> {
    constexpr static i8 ID = 4;
};

template <>
struct GetJsonTypeIDStrict<JsonType::JsonMap> {
    constexpr static i8 ID = 5;
};

template <>
struct GetJsonTypeIDStrict<JsonType::JsonNull> {
    constexpr static i8 ID = 6;
};

template <>
struct GetJsonTypeIDStrict<Json> {
    constexpr static i8 ID = 7;
};

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
struct JsonTypeTrait<JsonType::JsonInt> {
    using Type = JsonType::JsonInt;
    static constexpr auto name = "JsonInt";
};

template <>
struct JsonTypeTrait<JsonType::JsonFloat> {
    using Type = JsonType::JsonFloat;
    static constexpr auto name = "JsonFloat";
};

template <>
struct JsonTypeTrait<JsonType::JsonBool> {
    using Type = JsonType::JsonBool;
    static constexpr auto name = "JsonBool";
};

template <>
struct JsonTypeTrait<JsonType::JsonStr> {
    using Type = JsonType::JsonStr;
    static constexpr auto name = "JsonStr";
};

template <>
struct JsonTypeTrait<JsonType::JsonArray> {
    using Type = JsonType::JsonArray;
    static constexpr auto name = "JsonArray";
};

template <>
struct JsonTypeTrait<JsonType::JsonMap> {
    using Type = JsonType::JsonMap;
    static constexpr auto name = "JsonMap";
};

template <>
struct JsonTypeTrait<JsonType::JsonNull> {
    using Type = JsonType::JsonNull;
    static constexpr auto name = "JsonNull";
};

template <>
struct JsonTypeTrait<Json> {
    using Type = Json;
    static constexpr auto name = "Json";
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

// 哈希表类型映射
template <typename K, typename V>
struct JsonValueType<util::HashMap<K, V>> {
    using Type = std::conditional_t<
        std::is_convertible_v<K, util::String>,
        JsonType::JsonMap,
        void>;
    static_assert(
        !std::is_same_v<Type, void>,
        "JsonMap key must be convertible to util::String");
};

// Json类型映射
template <>
struct JsonValueType<Json> {
    using Type = Json;
};

} // namespace my::io

#endif // JSON_TRAIT_HPP