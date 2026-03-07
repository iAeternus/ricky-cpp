#ifndef JSON_SERIALIZER_HPP
#define JSON_SERIALIZER_HPP

#include "json_parser.hpp"
#include "serializer.hpp"

namespace my::json {

template <typename T>
concept SerializerType = requires(Json& json_mut, const T& obj, const Json& json, T& obj_mut) {
    { to_json(json_mut, obj) } -> std::convertible_to<void>;
    { from_json(json, obj_mut) } -> std::convertible_to<void>;
};

class JsonSerializer : public Serializer<JsonSerializer> {
public:
    using Self = JsonSerializer;
    using JsonString = str::String<>;
    using JsonStringView = str::StringView;

    static JsonString do_serialize(const i64 val) { return Json(val).dump(); }
    static JsonString do_serialize(const f64 val) { return Json(val).dump(); }
    static JsonString do_serialize(const bool val) { return Json(val).dump(); }
    static JsonString do_serialize(const JsonString& val) { return Json(val).dump(); }
    static JsonString do_serialize(const JsonStringView val) { return Json(val).dump(); }
    static JsonString do_serialize(const char* val) { return Json(val).dump(); }
    static JsonString do_serialize(const std::string_view val) { return Json(val).dump(); }
    static JsonString do_serialize(Null) { return Json().dump(); }

    template <typename T>
    static JsonString do_serialize(const util::Vec<T>& vec) {
        Json json;
        to_json(json, vec);
        return json.dump();
    }

    template <typename K, typename V>
    requires std::is_constructible_v<Json::String, K>
    static JsonString do_serialize(const util::HashMap<K, V>& map) {
        Json json;
        to_json(json, map);
        return json.dump();
    }

    template <SerializerType T>
    static JsonString do_serialize(const T& val) {
        Json json;
        to_json(json, val);
        return json.dump();
    }

    static void do_deserialize(const JsonStringView str, i64& res) {
        res = parse_json(str).into<i64>();
    }

    static void do_deserialize(const JsonStringView str, f64& res) {
        res = parse_json(str).into<f64>();
    }

    static void do_deserialize(const JsonStringView str, bool& res) {
        res = parse_json(str).into<bool>();
    }

    static void do_deserialize(const JsonStringView str, JsonString& res) {
        res = parse_json(str).into<JsonString>();
    }

    template <typename T>
    static void do_deserialize(const JsonStringView str, util::Vec<T>& res) {
        const auto json = parse_json(str);
        if (!json.is<JsonType::JsonArray>()) {
            throw type_exception("Expected JsonArray");
        }
        res.clear();
        const auto& arr = json.as_array();
        for (const auto& item : arr) {
            T value;
            if constexpr (SerializerType<T>) {
                from_json(item, value);
            } else {
                value = item.template into<T>();
            }
            res.push(std::move(value));
        }
    }

    template <typename K, typename V>
    requires std::is_constructible_v<K, Json::String>
    static void do_deserialize(const JsonStringView str, util::HashMap<K, V>& res) {
        const auto json = parse_json(str);
        if (!json.is<JsonType::JsonMap>()) {
            throw type_exception("Expected JsonMap");
        }
        res.clear();
        const auto& obj = json.as_object();
        for (const auto& [key, value] : obj) {
            V v;
            if constexpr (SerializerType<V>) {
                from_json(value, v);
            } else {
                v = value.template into<V>();
            }
            res.insert(K(key), std::move(v));
        }
    }

    template <SerializerType T>
    static void do_deserialize(const JsonStringView str, T& res) {
        const auto json = parse_json(str);
        from_json(json, res);
    }
};

// Vec 适配
template <typename T>
void to_json(Json& j, const util::Vec<T>& vec) {
    Json::Array arr;
    for (const auto& item : vec) {
        if constexpr (SerializerType<T>) {
            Json j_item;
            to_json(j_item, item);
            arr.push(std::move(j_item));
        } else {
            arr.push(Json(item));
        }
    }
    j = Json(std::move(arr));
}

// HashMap 适配
template <typename K, typename V>
requires std::is_constructible_v<Json::String, K>
void to_json(Json& j, const util::HashMap<K, V>& map) {
    Json::Map obj;
    for (const auto& [key, value] : map) {
        Json::String json_key(key);
        if constexpr (SerializerType<V>) {
            Json j_value;
            to_json(j_value, value);
            obj.insert(std::move(json_key), std::move(j_value));
        } else {
            obj.insert(std::move(json_key), Json(value));
        }
    }
    j = Json(std::move(obj));
}

// 基础类型
inline void to_json(Json& j, const i32 value) { j = Json(value); }
inline void to_json(Json& j, const i64 value) { j = Json(value); }
inline void to_json(Json& j, const f32 value) { j = Json(value); }
inline void to_json(Json& j, const f64 value) { j = Json(value); }
inline void to_json(Json& j, const bool value) { j = Json(value); }
inline void to_json(Json& j, const str::String<>& value) { j = Json(value); }
inline void to_json(Json& j, const str::StringView value) { j = Json(value); }
inline void to_json(Json& j, const char* value) { j = Json(value); }
inline void to_json(Json& j, const std::string_view value) { j = Json(value); }

} // namespace my::json

#endif // JSON_SERIALIZER_HPP
