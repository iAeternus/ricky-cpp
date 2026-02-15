#ifndef JSON_HPP
#define JSON_HPP

#include "json_trait.hpp"
#include "str_builder.hpp"
#include "my_exception.hpp"

namespace my::json {

class Json;

template <typename T>
void to_json(Json& j, const T& value);

class Json : public Object<Json> {
public:
    using Self = Json;
    using String = JsonType::JsonStr;
    using Array = JsonType::JsonArray;
    using Map = JsonType::JsonMap;

    Json() noexcept :
            kind_(JsonKind::Null) {}

    Json(Null) noexcept :
            kind_(JsonKind::Null) {}

    Json(bool value) noexcept :
            kind_(JsonKind::Bool) {
        storage_.b = value;
    }

    Json(i32 value) noexcept :
            kind_(JsonKind::Int) {
        storage_.i = static_cast<i64>(value);
    }

    Json(i64 value) noexcept :
            kind_(JsonKind::Int) {
        storage_.i = value;
    }

    Json(f32 value) noexcept :
            kind_(JsonKind::Float) {
        storage_.f = static_cast<f64>(value);
    }

    Json(f64 value) noexcept :
            kind_(JsonKind::Float) {
        storage_.f = value;
    }

    Json(const char* value) :
            Json(String(value)) {}

    Json(const String& value) :
            kind_(JsonKind::String) {
        new (&storage_.s) String(value);
    }

    Json(String&& value) noexcept :
            kind_(JsonKind::String) {
        new (&storage_.s) String(std::move(value));
    }

    Json(const Array& value) :
            kind_(JsonKind::Array) {
        new (&storage_.a) Array(value);
    }

    Json(Array&& value) noexcept :
            kind_(JsonKind::Array) {
        new (&storage_.a) Array(std::move(value));
    }

    Json(const Map& value) :
            kind_(JsonKind::Object) {
        new (&storage_.o) Map(value);
    }

    Json(Map&& value) noexcept :
            kind_(JsonKind::Object) {
        new (&storage_.o) Map(std::move(value));
    }

    Json(const Self& other) :
            kind_(JsonKind::Null) {
        copy_from(other);
    }

    Json(Self&& other) noexcept :
            kind_(JsonKind::Null) {
        move_from(std::move(other));
    }

    ~Json() {
        destroy();
    }

    Self& operator=(const Self& other) {
        if (this == &other) {
            return *this;
        }
        destroy();
        copy_from(other);
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        destroy();
        move_from(std::move(other));
        return *this;
    }

    static Json array() {
        return Json(Array{});
    }

    template <typename... Args>
    static Json array(Args&&... args) {
        Array arr;
        (arr.push(Json(std::forward<Args>(args))), ...);
        return Json(std::move(arr));
    }

    static Json object() {
        return Json(Map{});
    }

    template <typename... Args>
    static Json object(Args&&... args) {
        static_assert(sizeof...(Args) % 2 == 0, "Json::object requires even number of arguments");
        Map obj;
        build_object(obj, std::forward<Args>(args)...);
        return Json(std::move(obj));
    }

    JsonKind kind() const noexcept { return kind_; }

    bool is_null() const noexcept { return kind_ == JsonKind::Null; }
    bool is_bool() const noexcept { return kind_ == JsonKind::Bool; }
    bool is_int() const noexcept { return kind_ == JsonKind::Int; }
    bool is_float() const noexcept { return kind_ == JsonKind::Float; }
    bool is_number() const noexcept { return kind_ == JsonKind::Int || kind_ == JsonKind::Float; }
    bool is_string() const noexcept { return kind_ == JsonKind::String; }
    bool is_array() const noexcept { return kind_ == JsonKind::Array; }
    bool is_object() const noexcept { return kind_ == JsonKind::Object; }

    template <typename T>
    [[nodiscard]] bool is() const {
        if constexpr (!JsonValueType<T>::valid) {
            return false;
        } else {
            switch (JsonValueType<T>::kind) {
            case JsonKind::Null:
                return is_null();
            case JsonKind::Bool:
                return is_bool();
            case JsonKind::Int:
                return is_int();
            case JsonKind::Float:
                return is_float();
            case JsonKind::String:
                return is_string();
            case JsonKind::Array:
                return is_array();
            case JsonKind::Object:
                return is_object();
            }
        }
        return false;
    }

    bool& as_bool() { return require(JsonKind::Bool, storage_.b); }
    const bool& as_bool() const { return require(JsonKind::Bool, storage_.b); }

    i64& as_int() { return require(JsonKind::Int, storage_.i); }
    const i64& as_int() const { return require(JsonKind::Int, storage_.i); }

    f64& as_float() { return require(JsonKind::Float, storage_.f); }
    const f64& as_float() const { return require(JsonKind::Float, storage_.f); }

    String& as_string() { return require(JsonKind::String, storage_.s); }
    const String& as_string() const { return require(JsonKind::String, storage_.s); }

    Array& as_array() { return require(JsonKind::Array, storage_.a); }
    const Array& as_array() const { return require(JsonKind::Array, storage_.a); }

    Map& as_object() { return require(JsonKind::Object, storage_.o); }
    const Map& as_object() const { return require(JsonKind::Object, storage_.o); }

    Json* find(const String& key) {
        if (!is_object()) return nullptr;
        auto& obj = as_object();
        if (!obj.contains(key)) return nullptr;
        return &obj.get(key);
    }

    const Json* find(const String& key) const {
        if (!is_object()) return nullptr;
        const auto& obj = as_object();
        if (!obj.contains(key)) return nullptr;
        return &obj.get(key);
    }

    bool contains(const String& key) const {
        if (!is_object()) return false;
        return as_object().contains(key);
    }

    Json& operator[](const String& key) {
        return as_object().get(key);
    }

    const Json& operator[](const String& key) const {
        return as_object().get(key);
    }

    Json& operator[](usize index) {
        return as_array()[index];
    }

    const Json& operator[](usize index) const {
        return as_array()[index];
    }

    void push(Json value) {
        as_array().push(std::move(value));
    }

    void insert(String key, Json value) {
        as_object().insert(std::move(key), std::move(value));
    }

    usize size() const {
        if (is_array()) return as_array().len();
        if (is_object()) return as_object().size();
        throw runtime_exception("Json value is not array or object");
    }

    template <typename T>
    T into() const {
        using TargetType = typename JsonValueType<T>::Type;

        if constexpr (std::is_same_v<T, Json>) {
            return *this;
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonInt>) {
            if (!is_int()) {
                throw type_exception("Expected JsonInt");
            }
            return static_cast<T>(as_int());
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonFloat>) {
            if (is_float()) {
                return static_cast<T>(as_float());
            }
            if (is_int()) {
                return static_cast<T>(as_int());
            }
            throw type_exception("Expected JsonFloat");
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonBool>) {
            if (!is_bool()) {
                throw type_exception("Expected JsonBool");
            }
            return as_bool();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonStr>) {
            if (!is_string()) {
                throw type_exception("Expected JsonStr");
            }
            return as_string();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonArray>) {
            if (!is_array()) {
                throw type_exception("Expected JsonArray");
            }
            return into_array<T>();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonMap>) {
            if (!is_object()) {
                throw type_exception("Expected JsonMap");
            }
            return into_object<T>();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonNull>) {
            if (!is_null()) {
                throw type_exception("Expected JsonNull");
            }
            return JsonType::JsonNull{};
        }

        throw type_exception("Unsupported Json conversion");
    }

    util::String dump(i32 indent = 0) const {
        return dump_impl(indent, 1);
    }

    [[nodiscard]] CString __str__() const {
        return cstr(dump());
    }

private:
    union Storage {
        bool b;
        i64 i;
        f64 f;
        String s;
        Array a;
        Map o;
        Storage() {}
        ~Storage() {}
    } storage_;

    JsonKind kind_;

    void destroy() {
        switch (kind_) {
        case JsonKind::String:
            storage_.s.~String();
            break;
        case JsonKind::Array:
            storage_.a.~Array();
            break;
        case JsonKind::Object:
            storage_.o.~Map();
            break;
        default:
            break;
        }
        kind_ = JsonKind::Null;
    }

    void copy_from(const Self& other) {
        kind_ = other.kind_;
        switch (other.kind_) {
        case JsonKind::Null:
            break;
        case JsonKind::Bool:
            storage_.b = other.storage_.b;
            break;
        case JsonKind::Int:
            storage_.i = other.storage_.i;
            break;
        case JsonKind::Float:
            storage_.f = other.storage_.f;
            break;
        case JsonKind::String:
            new (&storage_.s) String(other.storage_.s);
            break;
        case JsonKind::Array:
            new (&storage_.a) Array(other.storage_.a);
            break;
        case JsonKind::Object:
            new (&storage_.o) Map(other.storage_.o);
            break;
        }
    }

    void move_from(Self&& other) {
        kind_ = other.kind_;
        switch (other.kind_) {
        case JsonKind::Null:
            break;
        case JsonKind::Bool:
            storage_.b = other.storage_.b;
            break;
        case JsonKind::Int:
            storage_.i = other.storage_.i;
            break;
        case JsonKind::Float:
            storage_.f = other.storage_.f;
            break;
        case JsonKind::String:
            new (&storage_.s) String(std::move(other.storage_.s));
            break;
        case JsonKind::Array:
            new (&storage_.a) Array(std::move(other.storage_.a));
            break;
        case JsonKind::Object:
            new (&storage_.o) Map(std::move(other.storage_.o));
            break;
        }
        other.destroy();
    }

    template <typename T>
    T& require(JsonKind kind, T& ref) {
        if (kind_ != kind) {
            throw type_exception("Json type mismatch");
        }
        return ref;
    }

    template <typename T>
    const T& require(JsonKind kind, const T& ref) const {
        if (kind_ != kind) {
            throw type_exception("Json type mismatch");
        }
        return ref;
    }

    template <typename Key, typename Value, typename... Rest>
    static void build_object(Map& obj, Key&& key, Value&& value, Rest&&... rest) {
        obj.insert(to_key(std::forward<Key>(key)), make_value(std::forward<Value>(value)));
        if constexpr (sizeof...(Rest) > 0) {
            build_object(obj, std::forward<Rest>(rest)...);
        }
    }

    template <typename Key>
    static String to_key(Key&& key) {
        if constexpr (std::is_convertible_v<Key, String>) {
            return String(std::forward<Key>(key));
        }
        return String(std::format("{}", key).c_str());
    }

    template <typename V>
    static Json make_value(V&& value) {
        if constexpr (std::is_convertible_v<V, Json>) {
            return Json(std::forward<V>(value));
        } else {
            Json j;
            to_json(j, std::forward<V>(value));
            return j;
        }
    }

    template <typename T>
    [[nodiscard]] auto into_array() const {
        using ValueType = typename T::value_t;
        util::Vec<ValueType> result;
        const auto& arr = as_array();
        for (const auto& item : arr) {
            result.push(item.template into<ValueType>());
        }
        return result;
    }

    template <typename T>
    [[nodiscard]] auto into_object() const {
        using KeyType = typename T::key_t;
        using ValueType = typename T::value_t;
        util::HashMap<KeyType, ValueType> result;
        const auto& obj = as_object();
        for (const auto& [key, value] : obj) {
            result.insert(key, value.template into<ValueType>());
        }
        return result;
    }

    static util::String dump_escape(const util::String& s) {
        util::StringBuilder sb;
        sb.append('"');
        auto bytes = s.into_string();
        for (unsigned char ch : bytes) {
            switch (ch) {
            case '"': sb.append("\\\""); break;
            case '\\': sb.append("\\\\"); break;
            case '\b': sb.append("\\b"); break;
            case '\f': sb.append("\\f"); break;
            case '\n': sb.append("\\n"); break;
            case '\r': sb.append("\\r"); break;
            case '\t': sb.append("\\t"); break;
            default:
                if (ch < 0x20) {
                    char buf[7];
                    std::snprintf(buf, sizeof(buf), "\\u%04X", static_cast<unsigned int>(ch));
                    sb.append(buf);
                } else {
                    sb.append(static_cast<char>(ch));
                }
                break;
            }
        }
        sb.append('"');
        return sb.build();
    }

    static util::String dump_array(const Array& arr, i32 indent, i32 depth) {
        if (arr.is_empty()) {
            return "[]";
        }
        util::StringBuilder sb;
        sb.append('[');
        bool first = true;
        util::String curr_indent(indent * depth, ' ');
        for (const auto& item : arr) {
            if (!first) sb.append(',');
            if (indent > 0) {
                sb.append('\n').append(curr_indent);
            }
            sb.append(item.dump_impl(indent, depth + 1));
            first = false;
        }
        if (indent > 0) {
            sb.append('\n').append(util::String(indent * (depth - 1), ' '));
        }
        sb.append(']');
        return sb.build();
    }

    static util::String dump_object(const Map& obj, i32 indent, i32 depth) {
        if (obj.empty()) {
            return "{}";
        }
        util::StringBuilder sb;
        sb.append('{');
        bool first = true;
        util::String curr_indent(indent * depth, ' ');
        for (const auto& [key, value] : obj) {
            if (!first) sb.append(',');
            if (indent > 0) {
                sb.append('\n').append(curr_indent);
            }
            sb.append(dump_escape(key)).append(':');
            if (indent > 0) sb.append(' ');
            sb.append(value.dump_impl(indent, depth + 1));
            first = false;
        }
        if (indent > 0) {
            sb.append('\n').append(util::String(indent * (depth - 1), ' '));
        }
        sb.append('}');
        return sb.build();
    }

    util::String dump_impl(i32 indent, i32 depth) const {
        switch (kind_) {
        case JsonKind::Null:
            return "null";
        case JsonKind::Bool:
            return storage_.b ? "true" : "false";
        case JsonKind::Int:
            return util::String::from_i64(storage_.i);
        case JsonKind::Float:
            return util::String::from_f64(storage_.f);
        case JsonKind::String:
            return dump_escape(storage_.s);
        case JsonKind::Array:
            return dump_array(storage_.a, indent, depth);
        case JsonKind::Object:
            return dump_object(storage_.o, indent, depth);
        }
        throw type_exception("Invalid Json kind");
    }
};

} // namespace my::json

#endif // JSON_HPP
