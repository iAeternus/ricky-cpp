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

namespace my::json {

class Json;

class Null {};

/**
 * @brief Json种类
 */
enum class JsonKind : u8 {
    Null,
    Bool,
    Int,
    Float,
    String,
    Array,
    Object
};

/**
 * @brief Json类型别名
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
 * @brief 类型转换映射
 */
template <typename T, typename = void>
struct JsonValueType {
    using Type = T;
    static constexpr JsonKind kind = JsonKind::Null;
    static constexpr bool valid = false;
};

// 整数类型映射到JsonInt
template <typename T>
struct JsonValueType<T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>>> {
    using Type = JsonType::JsonInt;
    static constexpr JsonKind kind = JsonKind::Int;
    static constexpr bool valid = true;
};

// 浮点类型映射到JsonFloat
template <typename T>
struct JsonValueType<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    using Type = JsonType::JsonFloat;
    static constexpr JsonKind kind = JsonKind::Float;
    static constexpr bool valid = true;
};

// 布尔类型映射
template <>
struct JsonValueType<bool> {
    using Type = JsonType::JsonBool;
    static constexpr JsonKind kind = JsonKind::Bool;
    static constexpr bool valid = true;
};

// 字符串类型映射
template <>
struct JsonValueType<util::String> {
    using Type = JsonType::JsonStr;
    static constexpr JsonKind kind = JsonKind::String;
    static constexpr bool valid = true;
};

// Null类型映射
template <>
struct JsonValueType<Null> {
    using Type = JsonType::JsonNull;
    static constexpr JsonKind kind = JsonKind::Null;
    static constexpr bool valid = true;
};

// Vec类型映射
template <typename T>
struct JsonValueType<util::Vec<T>> {
    using Type = JsonType::JsonArray;
    static constexpr JsonKind kind = JsonKind::Array;
    static constexpr bool valid = true;
};

// 哈希表类型映射
template <typename K, typename V>
struct JsonValueType<util::HashMap<K, V>> {
    using Type = std::conditional_t<std::is_convertible_v<K, util::String>, JsonType::JsonMap, void>;
    static_assert(!std::is_same_v<Type, void>, "JsonMap key must be convertible to util::String");
    static constexpr JsonKind kind = JsonKind::Object;
    static constexpr bool valid = true;
};

// Json类型映射
template <>
struct JsonValueType<Json> {
    using Type = Json;
    static constexpr JsonKind kind = JsonKind::Object;
    static constexpr bool valid = true;
};

} // namespace my::json

#endif // JSON_TRAIT_HPP
