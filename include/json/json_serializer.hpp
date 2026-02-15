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

    static util::String do_serialize(i64 val) { return Json(val).dump(); }
    static util::String do_serialize(f64 val) { return Json(val).dump(); }
    static util::String do_serialize(bool val) { return Json(val).dump(); }
    static util::String do_serialize(const util::String& val) { return Json(val).dump(); }
    static util::String do_serialize(Null) { return Json().dump(); }

    template <typename T>
    static util::String do_serialize(const util::Vec<T>& vec) {
        Json json;
        to_json(json, vec);
        return json.dump();
    }

    template <typename V>
    static util::String do_serialize(const util::HashMap<util::String, V>& map) {
        Json json;
        to_json(json, map);
        return json.dump();
    }

    template <SerializerType T>
    static util::String do_serialize(const T& val) {
        Json json;
        to_json(json, val);
        return json.dump();
    }

    static void do_deserialize(const util::String& str, i64& res) {
        res = parse_json(str).into<i64>();
    }

    static void do_deserialize(const util::String& str, f64& res) {
        res = parse_json(str).into<f64>();
    }

    static void do_deserialize(const util::String& str, bool& res) {
        res = parse_json(str).into<bool>();
    }

    static void do_deserialize(const util::String& str, util::String& res) {
        res = parse_json(str).into<util::String>();
    }

    template <typename T>
    static void do_deserialize(const util::String& str, util::Vec<T>& res) {
        auto json = parse_json(str);
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

    template <typename V>
    static void do_deserialize(const util::String& str, util::HashMap<util::String, V>& res) {
        auto json = parse_json(str);
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
            res.insert(key, std::move(v));
        }
    }

    template <SerializerType T>
    static void do_deserialize(const util::String& str, T& res) {
        auto json = parse_json(str);
        from_json(json, res);
    }
};

// Vec adapter
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

// HashMap adapter
template <typename V>
void to_json(Json& j, const util::HashMap<util::String, V>& map) {
    Json::Map obj;
    for (const auto& [key, value] : map) {
        if constexpr (SerializerType<V>) {
            Json j_value;
            to_json(j_value, value);
            obj.insert(key, std::move(j_value));
        } else {
            obj.insert(key, Json(value));
        }
    }
    j = Json(std::move(obj));
}

// Basic types
inline void to_json(Json& j, i32 value) { j = Json(value); }
inline void to_json(Json& j, i64 value) { j = Json(value); }
inline void to_json(Json& j, f32 value) { j = Json(value); }
inline void to_json(Json& j, f64 value) { j = Json(value); }
inline void to_json(Json& j, bool value) { j = Json(value); }
inline void to_json(Json& j, const util::String& value) { j = Json(value); }
inline void to_json(Json& j, const char* value) { j = Json(value); }

} // namespace my::json

#endif // JSON_SERIALIZER_HPP
