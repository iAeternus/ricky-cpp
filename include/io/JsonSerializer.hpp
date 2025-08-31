/**
 * @brief json序列化工具
 * @author Ricky
 * @date 2025/7/20
 * @version 1.0
 */
#ifndef JSON_SERIALIZER_HPP
#define JSON_SERIALIZER_HPP

#include "JsonParser.hpp"
#include "Serializer.hpp"

namespace my::io {

/**
 * @brief 支持序列化的类型
 */
template <typename T>
concept SerializerType = requires(Json& json_mut, const T& obj, const Json& json, T& obj_mut) {
    { to_json(json_mut, obj) } -> std::convertible_to<void>;
    { from_json(json, obj_mut) } -> std::convertible_to<void>;
};

/**
 * @brief json序列化工具
 */
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
        util::StringBuilder sb;
        sb.append('[');
        for (usize i = 0; i < vec.size(); ++i) {
            if (i > 0) sb.append(',');
            sb.append(JsonSerializer::do_serialize(vec[i]));
        }
        sb.append(']');
        return sb.build();
    }

    template <typename V>
    static util::String do_serialize(const util::Dict<util::String, V>& dict) {
        util::StringBuilder sb;
        sb.append('{');
        bool first = true;
        for (const auto& [key, value] : dict) {
            if (!first) sb.append(',');
            first = false;
            sb.append(JsonSerializer::do_serialize(key));
            sb.append(':');
            sb.append(JsonSerializer::do_serialize(value));
        }
        sb.append('}');
        return sb.build();
    }

    template <SerializerType T>
    static util::String do_serialize(const T& val) {
        Json json;
        to_json(json, val);
        return json.dump();
    }

    static void do_deserialize(const util::String& str, i64& res) {
        res = parse_json(str).into<JsonType::JsonInt>();
    }
    static void do_deserialize(const util::String& str, f64& res) {
        res = parse_json(str).into<JsonType::JsonFloat>();
    }
    static void do_deserialize(const util::String& str, bool& res) {
        res = parse_json(str).into<JsonType::JsonBool>();
    }
    static void do_deserialize(const util::String& str, util::String& res) {
        res = parse_json(str).into<JsonType::JsonStr>();
    }

    template <typename T>
    static void do_deserialize(const util::String& str, util::Vec<T>& res) {
        auto json = parse_json(str);
        if (!json.is<JsonType::JsonArray>()) {
            throw type_exception("Expected array for container");
        }
        res.clear();
        const auto& array = json.into<JsonType::JsonArray>();
        for (const auto& item : array) {
            T value;
            do_deserialize(item.dump(), value);
            res.append(std::move(value));
        }
    }

    template <typename V>
    static void do_deserialize(const util::String& str, util::Dict<util::String, V>& res) {
        auto json = parse_json(str);
        if (!json.is<JsonType::JsonDict>()) {
            throw type_exception("Expected dict for container");
        }
        res.clear();
        const auto& dict = json.into<JsonType::JsonDict>();
        for (const auto& [key, value] : dict) {
            V val;
            do_deserialize(value.dump(), val);
            res.insert(key, std::move(val));
        }
    }

    template <SerializerType T>
    static void do_deserialize(const util::String& str, T& res) {
        auto json = parse_json(str);
        from_json(json, res);
    }

private:
    static Json parse_json(const util::String& str) {
        return JsonParser::parse(str);
    }
};

#define DEFINE_JSON_ADAPTER(Type)                        \
    namespace my::io {                                   \
    inline void to_json(::my::io::Json&, const Type&);   \
    inline void from_json(const ::my::io::Json&, Type&); \
    }

// 为Vec提供适配器
template <typename T>
void to_json(Json& j, const util::Vec<T>& vec) {
    JsonType::JsonArray arr;
    for (const auto& item : vec) {
        Json j_item;
        to_json(j_item, item);
        arr.append(j_item);
    }
    j = std::move(arr);
}

// 为Dict提供适配器
template <typename V>
void to_json(Json& j, const util::Dict<util::String, V>& dict) {
    JsonType::JsonDict j_dict;
    for (const auto& [key, value] : dict) {
        Json j_value;
        to_json(j_value, value);
        j_dict.insert(key, j_value);
    }
    j = std::move(j_dict);
}

// 基础类型的 to_json 重载
inline void to_json(Json& j, i32 value) { j = Json(value); }
inline void to_json(Json& j, i64 value) { j = Json(value); }
inline void to_json(Json& j, f32 value) { j = Json(value); }
inline void to_json(Json& j, f64 value) { j = Json(value); }
inline void to_json(Json& j, bool value) { j = Json(value); }
inline void to_json(Json& j, const util::String& value) { j = Json(value); }
inline void to_json(Json& j, const char* value) { j = Json(value); }

} // namespace my::io

#endif // JSON_SERIALIZER_HPP