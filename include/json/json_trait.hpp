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

namespace my::json {

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
 * @brief 严格类型概念
 */
template <typename T>
concept JsonTypeStrictConcept = std::is_same_v<T, typename JsonType::JsonInt> || std::is_same_v<T, typename JsonType::JsonFloat> || std::is_same_v<T, typename JsonType::JsonBool> || std::is_same_v<T, typename JsonType::JsonStr> || std::is_same_v<T, typename JsonType::JsonArray> || std::is_same_v<T, typename JsonType::JsonDict> || std::is_same_v<T, typename JsonType::JsonNull>;

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

/**
 * @brief 可接受const reference volatile 修饰符获得Json类型的枚举值
 */
template <JsonTypeConcept T>
struct GetJsonTypeID {
    constexpr static i8 ID = GetJsonTypeIDStrict<std::remove_cvref_t<T>>::ID;
};

} // namespace my::json

#endif // JSON_TRAIT_HPP