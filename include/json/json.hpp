#ifndef JSON_HPP
#define JSON_HPP

#include "json_trait.hpp"

namespace my::json {

class Json;

template <typename T>
void to_json(Json& j, const T& value);

class Json : public Object<Json> {
public:
    using Self = Json;
    using String = JsonType::JsonStr;
    using StringView = str::StringView;
    using Array = JsonType::JsonArray;
    using Map = JsonType::JsonMap;

    Json() noexcept : kind_(JsonKind::Null) {}
    Json(Null) noexcept : kind_(JsonKind::Null) {}

    Json(const bool value) noexcept : kind_(JsonKind::Bool) { storage_.b = value; }
    Json(const i32 value) noexcept : kind_(JsonKind::Int) { storage_.i = static_cast<i64>(value); }
    Json(const i64 value) noexcept : kind_(JsonKind::Int) { storage_.i = value; }
    Json(const f32 value) noexcept : kind_(JsonKind::Float) { storage_.f = static_cast<f64>(value); }
    Json(const f64 value) noexcept : kind_(JsonKind::Float) { storage_.f = value; }

    Json(const char* value) : Json(String(value)) {}
    Json(const StringView value) : Json(String(value)) {}
    Json(const std::string_view value) : Json(String(value.data(), value.size())) {}

    Json(const String& value) : kind_(JsonKind::String) {
        new (&storage_.s) String(value);
    }

    Json(String&& value) noexcept : kind_(JsonKind::String) {
        new (&storage_.s) String(std::move(value));
    }

    Json(const Array& value) : kind_(JsonKind::Array) {
        new (&storage_.a) Array(value);
    }

    Json(Array&& value) noexcept : kind_(JsonKind::Array) {
        new (&storage_.a) Array(std::move(value));
    }

    Json(const Map& value) : kind_(JsonKind::Object) {
        new (&storage_.o) Map(value);
    }

    Json(Map&& value) noexcept : kind_(JsonKind::Object) {
        new (&storage_.o) Map(std::move(value));
    }

    Json(const Self& other) : kind_(JsonKind::Null) {
        copy_from(other);
    }

    Json(Self&& other) noexcept : kind_(JsonKind::Null) {
        move_from(std::move(other));
    }

    ~Json() {
        destroy();
    }

    auto operator=(const Self& other) -> Self& {
        if (this == &other) return *this;
        destroy();
        copy_from(other);
        return *this;
    }

    auto operator=(Self&& other) noexcept -> Self& {
        if (this == &other) return *this;
        destroy();
        move_from(std::move(other));
        return *this;
    }

    static auto array() -> Json {
        return Json(Array{});
    }

    template <typename... Args>
    static auto array(Args&&... args) -> Json {
        Array arr;
        (arr.push(Json(std::forward<Args>(args))), ...);
        return Json(std::move(arr));
    }

    static auto object() -> Json {
        return Json(Map{});
    }

    template <typename... Args>
    static auto object(Args&&... args) -> Json {
        static_assert(sizeof...(Args) % 2 == 0, "Json::object requires even number of arguments");
        Map obj;
        build_object(obj, std::forward<Args>(args)...);
        return Json(std::move(obj));
    }

    [[nodiscard]] auto kind() const noexcept -> JsonKind { return kind_; }

    [[nodiscard]] auto is_null() const noexcept -> bool { return kind_ == JsonKind::Null; }
    [[nodiscard]] auto is_bool() const noexcept -> bool { return kind_ == JsonKind::Bool; }
    [[nodiscard]] auto is_int() const noexcept -> bool { return kind_ == JsonKind::Int; }
    [[nodiscard]] auto is_float() const noexcept -> bool { return kind_ == JsonKind::Float; }
    [[nodiscard]] auto is_number() const noexcept -> bool { return kind_ == JsonKind::Int || kind_ == JsonKind::Float; }
    [[nodiscard]] auto is_string() const noexcept -> bool { return kind_ == JsonKind::String; }
    [[nodiscard]] auto is_array() const noexcept -> bool { return kind_ == JsonKind::Array; }
    [[nodiscard]] auto is_object() const noexcept -> bool { return kind_ == JsonKind::Object; }

    template <typename T>
    [[nodiscard]] auto is() const -> bool {
        if constexpr (!JsonValueType<T>::valid) {
            return false;
        } else {
            switch (JsonValueType<T>::kind) {
            case JsonKind::Null: return is_null();
            case JsonKind::Bool: return is_bool();
            case JsonKind::Int: return is_int();
            case JsonKind::Float: return is_float();
            case JsonKind::String: return is_string();
            case JsonKind::Array: return is_array();
            case JsonKind::Object: return is_object();
            }
        }
        return false;
    }

    auto as_bool() -> bool& { return require(JsonKind::Bool, storage_.b); }
    auto as_bool() const -> const bool& { return require(JsonKind::Bool, storage_.b); }

    auto as_int() -> i64& { return require(JsonKind::Int, storage_.i); }
    auto as_int() const -> const i64& { return require(JsonKind::Int, storage_.i); }

    auto as_float() -> f64& { return require(JsonKind::Float, storage_.f); }
    auto as_float() const -> const f64& { return require(JsonKind::Float, storage_.f); }

    auto as_string() -> String& { return require(JsonKind::String, storage_.s); }
    auto as_string() const -> const String& { return require(JsonKind::String, storage_.s); }

    auto as_array() -> Array& { return require(JsonKind::Array, storage_.a); }
    auto as_array() const -> const Array& { return require(JsonKind::Array, storage_.a); }

    auto as_object() -> Map& { return require(JsonKind::Object, storage_.o); }
    auto as_object() const -> const Map& { return require(JsonKind::Object, storage_.o); }

    auto find(const String& key) -> Json* {
        if (!is_object()) return nullptr;
        auto& obj = as_object();
        if (!obj.contains(key)) return nullptr;
        return &obj.get(key);
    }

    auto find(const String& key) const -> const Json* {
        if (!is_object()) return nullptr;
        const auto& obj = as_object();
        if (!obj.contains(key)) return nullptr;
        return &obj.get(key);
    }

    auto find(const StringView key) -> Json* { return find(String(key)); }
    auto find(const StringView key) const -> const Json* { return find(String(key)); }
    auto find(const std::string_view key) -> Json* { return find(String(key.data(), key.size())); }
    auto find(const std::string_view key) const -> const Json* { return find(String(key.data(), key.size())); }

    [[nodiscard]] auto contains(const String& key) const -> bool {
        if (!is_object()) return false;
        return as_object().contains(key);
    }

    [[nodiscard]] auto contains(const StringView key) const -> bool { return contains(String(key)); }
    [[nodiscard]] auto contains(const std::string_view key) const -> bool { return contains(String(key.data(), key.size())); }

    auto operator[](const String& key) -> Json& { return as_object().get(key); }
    auto operator[](const String& key) const -> const Json& { return as_object().get(key); }

    auto operator[](const StringView key) -> Json& { return (*this)[String(key)]; }
    auto operator[](const StringView key) const -> const Json& { return (*this)[String(key)]; }
    auto operator[](const std::string_view key) -> Json& { return (*this)[String(key.data(), key.size())]; }
    auto operator[](const std::string_view key) const -> const Json& { return (*this)[String(key.data(), key.size())]; }

    auto operator[](const usize index) -> Json& { return as_array()[index]; }
    auto operator[](const usize index) const -> const Json& { return as_array()[index]; }

    template <std::integral I>
        requires(!std::same_as<std::remove_cvref_t<I>, bool>)
    auto operator[](const I index) -> Json& {
        return as_array()[static_cast<usize>(index)];
    }

    template <std::integral I>
        requires(!std::same_as<std::remove_cvref_t<I>, bool>)
    auto operator[](const I index) const -> const Json& {
        return as_array()[static_cast<usize>(index)];
    }

    auto push(Json value) -> void { as_array().push(std::move(value)); }

    auto insert(String key, Json value) -> void {
        as_object().insert(std::move(key), std::move(value));
    }

    auto insert(const StringView key, Json value) -> void {
        insert(String(key), std::move(value));
    }

    auto insert(const std::string_view key, Json value) -> void {
        insert(String(key.data(), key.size()), std::move(value));
    }

    [[nodiscard]] auto size() const -> usize {
        if (is_array()) return as_array().len();
        if (is_object()) return as_object().size();
        throw runtime_exception("Json value is not array or object");
    }

    template <typename T>
    auto into() const -> T {
        using TargetType = typename JsonValueType<T>::Type;

        if constexpr (std::is_same_v<T, Json>) {
            return *this;
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonInt>) {
            if (!is_int()) throw type_exception("Expected JsonInt");
            return static_cast<T>(as_int());
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonFloat>) {
            if (is_float()) return static_cast<T>(as_float());
            if (is_int()) return static_cast<T>(as_int());
            throw type_exception("Expected JsonFloat");
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonBool>) {
            if (!is_bool()) throw type_exception("Expected JsonBool");
            return as_bool();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonStr>) {
            if (!is_string()) throw type_exception("Expected JsonStr");
            if constexpr (std::is_same_v<T, str::StringView>) {
                return as_string().as_str();
            }
            if constexpr (std::is_same_v<T, std::string_view>) {
                return as_string().as_str().to_std_string_view();
            }
            if constexpr (std::is_same_v<T, std::string>) {
                return as_string().as_str().to_std_string();
            }
            return as_string();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonArray>) {
            if (!is_array()) throw type_exception("Expected JsonArray");
            return into_array<T>();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonMap>) {
            if (!is_object()) throw type_exception("Expected JsonMap");
            return into_object<T>();
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonNull>) {
            if (!is_null()) throw type_exception("Expected JsonNull");
            return JsonType::JsonNull{};
        }

        throw type_exception("Unsupported Json conversion");
    }

    [[nodiscard]] auto dump(const i32 indent = 0) const -> String {
        return dump_impl(indent, 1);
    }

    [[nodiscard]] auto to_string() const -> CString {
        const auto text = dump();
        return CString(reinterpret_cast<const char*>(text.as_bytes()), text.len());
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

    auto destroy() -> void {
        switch (kind_) {
        case JsonKind::String: storage_.s.~String(); break;
        case JsonKind::Array: storage_.a.~Array(); break;
        case JsonKind::Object: storage_.o.~Map(); break;
        default: break;
        }
        kind_ = JsonKind::Null;
    }

    auto copy_from(const Self& other) -> void {
        kind_ = other.kind_;
        switch (other.kind_) {
        case JsonKind::Null: break;
        case JsonKind::Bool: storage_.b = other.storage_.b; break;
        case JsonKind::Int: storage_.i = other.storage_.i; break;
        case JsonKind::Float: storage_.f = other.storage_.f; break;
        case JsonKind::String: new (&storage_.s) String(other.storage_.s); break;
        case JsonKind::Array: new (&storage_.a) Array(other.storage_.a); break;
        case JsonKind::Object: new (&storage_.o) Map(other.storage_.o); break;
        }
    }

    auto move_from(Self&& other) -> void {
        kind_ = other.kind_;
        switch (other.kind_) {
        case JsonKind::Null: break;
        case JsonKind::Bool: storage_.b = other.storage_.b; break;
        case JsonKind::Int: storage_.i = other.storage_.i; break;
        case JsonKind::Float: storage_.f = other.storage_.f; break;
        case JsonKind::String: new (&storage_.s) String(std::move(other.storage_.s)); break;
        case JsonKind::Array: new (&storage_.a) Array(std::move(other.storage_.a)); break;
        case JsonKind::Object: new (&storage_.o) Map(std::move(other.storage_.o)); break;
        }
        other.destroy();
    }

    template <typename T>
    auto require(const JsonKind kind, T& ref) -> T& {
        if (kind_ != kind) throw type_exception("Json type mismatch");
        return ref;
    }

    template <typename T>
    auto require(const JsonKind kind, const T& ref) const -> const T& {
        if (kind_ != kind) throw type_exception("Json type mismatch");
        return ref;
    }

    template <typename Key, typename Value, typename... Rest>
    static auto build_object(Map& obj, Key&& key, Value&& value, Rest&&... rest) -> void {
        obj.insert(to_key(std::forward<Key>(key)), make_value(std::forward<Value>(value)));
        if constexpr (sizeof...(Rest) > 0) {
            build_object(obj, std::forward<Rest>(rest)...);
        }
    }

    template <typename Key>
    static auto to_key(Key&& key) -> String {
        using K = std::remove_cvref_t<Key>;
        if constexpr (std::is_same_v<K, String>) {
            return std::forward<Key>(key);
        } else if constexpr (std::is_same_v<K, StringView>) {
            return String(key);
        } else if constexpr (std::is_same_v<K, std::string_view>) {
            return String(key.data(), key.size());
        } else if constexpr (std::is_convertible_v<Key, const char*>) {
            return String(std::forward<Key>(key));
        } else if constexpr (std::is_constructible_v<String, Key>) {
            return String(std::forward<Key>(key));
        } else {
            const auto key_text = std::format("{}", key);
            return String(key_text.c_str(), key_text.size());
        }
    }

    template <typename V>
    static auto make_value(V&& value) -> Json {
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
            result.insert(KeyType(key), value.template into<ValueType>());
        }
        return result;
    }

    static auto append_literal(String& out, const char* literal) -> void {
        out.push_str(StringView(literal));
    }

    static auto append_spaces(String& out, const i32 count) -> void {
        for (i32 i = 0; i < count; ++i) {
            out.push(static_cast<char32_t>(' '));
        }
    }

    static auto dump_escape(const String& str) -> String {
        String out;
        out.push(static_cast<char32_t>('"'));
        for (const auto ch : str.chars()) {
            switch (ch) {
            case '"': append_literal(out, "\\\""); break;
            case '\\': append_literal(out, "\\\\"); break;
            case '\b': append_literal(out, "\\b"); break;
            case '\f': append_literal(out, "\\f"); break;
            case '\n': append_literal(out, "\\n"); break;
            case '\r': append_literal(out, "\\r"); break;
            case '\t': append_literal(out, "\\t"); break;
            default:
                if (ch < 0x20) {
                    const auto esc = std::format("\\u{:04X}", static_cast<unsigned int>(ch));
                    out.push_str(StringView(esc.c_str(), esc.size()));
                } else {
                    out.push(ch);
                }
                break;
            }
        }
        out.push(static_cast<char32_t>('"'));
        return out;
    }

    static auto dump_array(const Array& arr, const i32 indent, const i32 depth) -> String {
        if (arr.is_empty()) {
            return String("[]");
        }

        String out;
        out.push(static_cast<char32_t>('['));
        bool first = true;
        for (const auto& item : arr) {
            if (!first) out.push(static_cast<char32_t>(','));
            if (indent > 0) {
                out.push(static_cast<char32_t>('\n'));
                append_spaces(out, indent * depth);
            }
            const String item_text = item.dump_impl(indent, depth + 1);
            out.push_str(item_text.as_str());
            first = false;
        }

        if (indent > 0) {
            out.push(static_cast<char32_t>('\n'));
            append_spaces(out, indent * (depth - 1));
        }
        out.push(static_cast<char32_t>(']'));
        return out;
    }

    static auto dump_object(const Map& obj, const i32 indent, const i32 depth) -> String {
        if (obj.empty()) {
            return String("{}");
        }

        String out;
        out.push(static_cast<char32_t>('{'));
        bool first = true;
        for (const auto& [key, value] : obj) {
            if (!first) out.push(static_cast<char32_t>(','));
            if (indent > 0) {
                out.push(static_cast<char32_t>('\n'));
                append_spaces(out, indent * depth);
            }

            const String key_text = dump_escape(key);
            out.push_str(key_text.as_str());
            out.push(static_cast<char32_t>(':'));
            if (indent > 0) out.push(static_cast<char32_t>(' '));

            const String value_text = value.dump_impl(indent, depth + 1);
            out.push_str(value_text.as_str());
            first = false;
        }

        if (indent > 0) {
            out.push(static_cast<char32_t>('\n'));
            append_spaces(out, indent * (depth - 1));
        }
        out.push(static_cast<char32_t>('}'));
        return out;
    }

    [[nodiscard]] auto dump_impl(const i32 indent, const i32 depth) const -> String {
        switch (kind_) {
        case JsonKind::Null:
            return String("null");
        case JsonKind::Bool:
            return storage_.b ? String("true") : String("false");
        case JsonKind::Int: {
            const auto text = std::format("{}", storage_.i);
            return String(text.c_str());
        }
        case JsonKind::Float: {
            const auto text = std::format("{}", storage_.f);
            return String(text.c_str());
        }
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
