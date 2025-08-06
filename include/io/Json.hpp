/**
 * @brief Json对象
 * @author Ricky
 * @date 2025/1/11
 * @version 1.0
 */
#ifndef JSON_HPP
#define JSON_HPP

#include "json_trait.hpp"
#include "StringBuilder.hpp"

namespace my::io {

class Json : public Object<Json> {
public:
    using Self = Json;

    Json() :
            Json(Null{}) {}

    // 基础类型构造函数
    Json(i32 value) :
            Json(static_cast<JsonType::JsonInt>(value)) {}

    Json(f32 value) :
            Json(static_cast<JsonType::JsonFloat>(value)) {}

    Json(const char* value) :
            Json(util::String(value)) {}

    /**
     * @brief Json对象构造
     * @param item Json对象内容
     */
    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Json>>>
    Json(T&& item) {
        using DecayT = std::decay_t<T>;
        using MappedType = typename JsonValueType<DecayT>::Type;

        if constexpr (std::is_same_v<MappedType, JsonType::JsonArray>) {
            json_type_ = GetJsonTypeIDStrict<JsonType::JsonArray>::ID;
            auto* arr = new JsonType::JsonArray();
            for (auto&& elem : item) {
                arr->append(Json(std::forward<decltype(elem)>(elem)));
            }
            json_item_ = arr;
        } else if constexpr (std::is_same_v<MappedType, JsonType::JsonDict>) {
            json_type_ = GetJsonTypeIDStrict<JsonType::JsonDict>::ID;
            auto* dict = new JsonType::JsonDict();
            for (auto&& [key, value] : item) {
                dict->insert(key, Json(std::forward<decltype(value)>(value)));
            }
            json_item_ = dict;
        } else {
            json_type_ = GetJsonTypeIDStrict<MappedType>::ID;
            json_item_ = new MappedType(std::forward<T>(item));
        }
    }

    Json(const Self& other) :
            json_type_(other.json_type_) {
        switch (json_type_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            this->json_item_ = new JsonType::JsonInt(other.into<JsonType::JsonInt>());
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            this->json_item_ = new JsonType::JsonFloat(other.into<JsonType::JsonFloat>());
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            this->json_item_ = new JsonType::JsonBool(other.into<JsonType::JsonBool>());
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            this->json_item_ = new JsonType::JsonStr(other.into<JsonType::JsonStr>());
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            this->json_item_ = new JsonType::JsonArray(other.into<JsonType::JsonArray>());
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            this->json_item_ = new JsonType::JsonDict(other.into<JsonType::JsonDict>());
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            this->json_item_ = new JsonType::JsonNull(other.into<JsonType::JsonNull>());
            break;
        default:
            throw type_exception("json type {} is not valid", SRC_LOC, json_type_);
        }
    }

    Json(Self&& other) noexcept :
            json_type_(other.json_type_), json_item_(other.json_item_) {
        other.json_type_ = GetJsonTypeIDStrict<JsonType::JsonNull>::ID;
        other.json_item_ = nullptr;
    }

    ~Json() {
        release();
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        release();
        json_type_ = other.json_type_;

        switch (json_type_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            this->json_item_ = new JsonType::JsonInt(other.into<JsonType::JsonInt>());
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            this->json_item_ = new JsonType::JsonFloat(other.into<JsonType::JsonFloat>());
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            this->json_item_ = new JsonType::JsonBool(other.into<JsonType::JsonBool>());
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            this->json_item_ = new JsonType::JsonStr(other.into<JsonType::JsonStr>());
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            this->json_item_ = new JsonType::JsonArray(other.into<JsonType::JsonArray>());
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            this->json_item_ = new JsonType::JsonDict(other.into<JsonType::JsonDict>());
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            this->json_item_ = new JsonType::JsonNull(other.into<JsonType::JsonNull>());
            break;
        default:
            throw type_exception("json type {} is not valid", SRC_LOC, json_type_);
        }
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        release();
        this->json_type_ = other.json_type_;
        this->json_item_ = other.json_item_;

        other.json_type_ = GetJsonTypeID<JsonType::JsonNull>::ID;
        other.json_item_ = nullptr;
        return *this;
    }

    /**
     * @brief 创建JSON对象
     * @param args 键值对列表，支持多种类型自动转换
     */
    template <typename... Args>
    static Json object(Args&&... args) {
        static_assert(sizeof...(Args) % 2 == 0, "Json::object requires even number of arguments");
        JsonType::JsonDict dict;
        build_dict(dict, std::forward<Args>(args)...);
        return Json(std::move(dict));
    }

    /**
     * @brief 创建JSON数组
     * @param args 元素列表，支持多种类型自动转换
     */
    template <typename... Args>
    static Json array(Args&&... args) {
        JsonType::JsonArray arr;
        arr.append(to_json_object(std::forward<Args>(args))...);
        return Json(std::move(arr));
    }

    /**
     * @brief 交换两个Json对象
     * @param other 要交换的Json对象
     * @note 该函数 noexcept
     */
    void swap(Self& other) noexcept {
        std::swap(this->json_type_, other.json_type_);
        std::swap(this->json_item_, other.json_item_);
    }

    template <typename T>
    bool is() const {
        using TargetType = typename JsonValueType<T>::Type;
        return json_type_ == GetJsonTypeID<TargetType>::ID;
    }

    /**
     * @brief 返回Json存储的对象类型名
     */
    static CString type_name(i8 id) {
        switch (id) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            return "JsonInt";
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            return "JsonFloat";
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            return "JsonBool";
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            return "JsonStr";
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            return "JsonArray";
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            return "JsonDict";
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            return "JsonNull";
        default:
            throw type_exception("json type {} is not valid", SRC_LOC, id);
        }
    }

    /**
     * @brief 将Json对象序列化为字符串
     * @param indent 缩进
     * @return 序列化后的JSON字符串
     */
    [[nodiscard]] util::String dump(i32 indent = 0) const {
        return dump_impl(indent, 1);
    }

    /**
     * @brief 将Json对象转换为指定类型的指针
     * @tparam T 要转换的类型
     * @return 如果Json对象的类型是T类型，返回该对象的指针，否则抛出type_exception
     * @note 该函数不检查指针的有效性
     */
    template <typename T>
    T* into_ptr() const {
        using TargetType = typename JsonValueType<T>::Type;
        if (!is<TargetType>()) {
            throw type_exception("expected {} but got {}", SRC_LOC, JsonTypeTrait<TargetType>::name, type_name(json_type_));
        }
        return static_cast<T*>(json_item_);
    }

    /**
     * @brief 将Json对象转换为指定类型
     * @tparam T 要转换的类型
     * @return 如果Json对象的类型是T类型，返回该对象，否则抛出type_exception
     */
    template <typename T>
    T into() const {
        if constexpr (std::is_same_v<T, Json>) {
            return *this;
        }

        using TargetType = typename JsonValueType<T>::Type;
        constexpr i8 target_id = GetJsonTypeID<TargetType>::ID;

        if (json_type_ != target_id) {
            throw type_exception("expected {} but got {}", SRC_LOC, JsonTypeTrait<TargetType>::name, type_name(json_type_));
        }

        if constexpr (std::is_same_v<TargetType, JsonType::JsonArray>) {
            return into_array<T>();
        } else if constexpr (std::is_same_v<TargetType, JsonType::JsonDict>) {
            return into_dict<T>();
        } else {
            return into_basic<T, TargetType>();
        }
    }

    /**
     * @brief 键访问，需要当前Json对象为JsonDict类型
     */
    Self& operator[](const util::String& key) {
        return into<JsonType::JsonDict>().get(key);
    }

    /**
     * @brief 键访问，需要当前Json对象为JsonDict类型（常量版本）
     */
    const Self& operator[](const util::String& key) const {
        return into<JsonType::JsonDict>().get(key);
    }

    /**
     * @brief 通过键获取Json对象，需要当前Json对象为JsonDict类型
     * @param key 键
     * @return 该键对应的Json对象
     * @throw std::out_of_range 如果键不存在
     * @throw type_exception 如果该键对应的Json对象不能转换为指定类型
     */
    template <typename T>
    T get(const util::String& key) const {
        return into<JsonType::JsonDict>().get(key).into<T>();
    }

    /**
     * @brief 尾部添加一个Json对象，需要当前Json对象为JsonArray类型
     */
    Self& append(const Self& json) {
        into<JsonType::JsonArray>().append(json);
        return *this;
    }

    /**
     * @brief 下标访问，需要当前Json对象为JsonArray类型
     * @param index 索引，从0开始
     */
    Self& operator[](usize index) {
        return into<JsonType::JsonArray>()[index];
    }

    /**
     * @brief 下标访问，需要当前Json对象为JsonArray类型（常量版本）
     * @param index 索引，从0开始
     */
    const Self& operator[](usize index) const {
        return into<JsonType::JsonArray>()[index];
    }

    /**
     * @brief 获取指定索引位置的Json对象并转换为指定类型
     * @tparam T 要转换的类型
     * @param index 索引，从0开始
     * @return 指定索引位置的Json对象转换后的值
     * @throw std::out_of_range 如果索引超出范围
     * @throw type_exception 如果Json对象不能转换为指定类型
     */
    template <typename T>
    T get(usize index) const {
        return into<JsonType::JsonArray>()[index].into<T>();
    }

    /**
     * @brief 移除指定位置的Json对象，需要当前Json对象为JsonArray类型
     * @param index 索引，从0开始。若为-1，则移除最后一个元素
     * @return void
     */
    void pop(isize index = -1) {
        into<JsonType::JsonArray>().pop(index);
    }

    /**
     * @brief 获取json对象的长度，需要当前Json对象为JsonArray或JsonDict类型
     */
    usize size() const {
        if (is<JsonType::JsonArray>()) {
            return into<JsonType::JsonArray>().size();
        }
        if (is<JsonType::JsonDict>()) {
            return into<JsonType::JsonDict>().size();
        }
        throw runtime_exception("json type is not JSON_ARRAY or JSON_DICT");
    }

    [[nodiscard]] CString __str__() const {
        switch (json_type_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            return cstr(into<JsonType::JsonInt>());
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            return cstr(into<JsonType::JsonFloat>());
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            return cstr(into<JsonType::JsonBool>());
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            using namespace my::util;
            return cstr("\""_s + into<JsonType::JsonStr>() + "\""_s);
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            return cstr(into<JsonType::JsonArray>());
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            return cstr(into<JsonType::JsonDict>());
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            return cstr("null");
        default:
            throw type_exception("json type {} is not valid", SRC_LOC, json_type_);
            break;
        }
    }

private:
    /**
     * @brief 释放内存
     */
    void release() {
        switch (json_type_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            delete into_ptr<JsonType::JsonInt>();
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            delete into_ptr<JsonType::JsonFloat>();
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            delete into_ptr<JsonType::JsonBool>();
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            delete into_ptr<JsonType::JsonStr>();
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            delete into_ptr<JsonType::JsonArray>();
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            delete into_ptr<JsonType::JsonDict>();
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            delete into_ptr<JsonType::JsonNull>();
            break;
        default:
            throw type_exception("json type {} is not valid", SRC_LOC, json_type_);
        }
    }

    // 递归构建字典
    template <typename Key, typename Value, typename... Rest>
    static void build_dict(JsonType::JsonDict& dict, Key&& key, Value&& value, Rest&&... rest) {
        util::String key_str;
        if constexpr (std::is_convertible_v<Key, util::String>) {
            key_str = util::String(std::forward<Key>(key));
        } else {
            // 添加其他类型的键转换支持
            key_str = util::String(std::format("{}", key));
        }

        dict.insert(
            std::move(key_str),
            to_json_object(std::forward<Value>(value)));

        if constexpr (sizeof...(Rest) > 0) {
            build_dict(dict, std::forward<Rest>(rest)...);
        }
    }

    // 将任意类型转换为JSON对象
    template <typename T>
    static Json to_json_object(T&& value) {
        if constexpr (std::is_convertible_v<T, Json>) {
            return Json(std::forward<T>(value));
        } else {
            Json result;
            to_json(result, std::forward<T>(value));
            return result;
        }
    }

    template <typename T>
    auto into_array() const {
        using ValueType = typename T::value_t;
        util::Vec<ValueType> result;
        const auto& arr = *static_cast<JsonType::JsonArray*>(json_item_);
        for (const auto& item : arr) {
            result.append(item.template into<ValueType>());
        }
        return result;
    }

    // 提取字典转换逻辑
    template <typename T>
    auto into_dict() const {
        using KeyType = typename T::key_t;
        using ValueType = typename T::value_t;
        util::Dict<KeyType, ValueType> result;
        const auto& dict = *static_cast<JsonType::JsonDict*>(json_item_);
        for (const auto& [key, value] : dict) {
            result.insert(key, value.template into<ValueType>());
        }
        return result;
    }

    // 提取基础类型转换逻辑
    template <typename T, typename TargetType>
    auto into_basic() const {
        const auto* ptr = static_cast<TargetType*>(json_item_);
        if constexpr (!std::is_same_v<T, TargetType>) {
            return static_cast<T>(*ptr);
        } else {
            return *ptr;
        }
    }

    util::String dump_impl(i32 indent, i32 depth) const {
        switch (json_type_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            return util::String::from_i64(into<JsonType::JsonInt>());
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            return util::String::from_f64(into<JsonType::JsonFloat>());
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            return into<JsonType::JsonBool>() ? "true" : "false";
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            return escape_string(into<JsonType::JsonStr>());
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            return dump_array(into<JsonType::JsonArray>(), indent, depth);
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            return dump_dict(into<JsonType::JsonDict>(), indent, depth);
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            return "null";
        default:
            throw type_exception("json type {} is not valid", SRC_LOC, json_type_);
        }
    }

    /**
     * @brief 字符串转义处理
     */
    static util::String escape_string(const JsonType::JsonStr& str) {
        util::StringBuilder res;
        res.append('\"');
        for (const auto& cp : str) {
            if (cp == '\"') {
                res.append("\\\"");
            } else if (cp == '\\') {
                res.append("\\\\");
            } else if (cp == '\b') {
                res.append("\\b");
            } else if (cp == '\f') {
                res.append("\\f");
            } else if (cp == '\n') {
                res.append("\\n");
            } else if (cp == '\r') {
                res.append("\\r");
            } else if (cp == '\t') {
                res.append("\\t");
            } else {
                if (cp < i2c(0x20)) { // 控制字符
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%4s", cp.data());
                    res.append(buf);
                } else {
                    res.append(cp);
                }
            }
        }
        res.append('\"');
        return res.build();
    }

    /**
     * @brief 序列化数组
     */
    static util::String dump_array(const JsonType::JsonArray& arr, i32 indent, i32 depth) {
        if (arr.empty()) return "[]";

        util::StringBuilder res;
        res.append('[');

        util::String curr_indent(indent * depth);
        bool first = true;
        for (const auto& item : arr) {
            if (!first) res.append(',');
            if (indent > 0) {
                res.append('\n').append(curr_indent);
            }

            res.append(item.dump_impl(indent, depth + 1));
            first = false;
        }

        res.append('\n').append(util::String(indent * (depth - 1))).append(']');
        return res.build();
    }

    /**
     * @brief 序列化字典
     */
    static util::String dump_dict(const JsonType::JsonDict& dict, i32 indent, i32 depth) {
        if (dict.empty()) return "{}";

        util::StringBuilder res;
        res.append('{');

        util::String curr_indent(indent * depth);
        bool first = true;
        for (const auto& [key, value] : dict) {
            if (!first) res.append(',');
            if (indent > 0) {
                res.append('\n').append(curr_indent);
            }

            res.append(escape_string(key)).append(':');
            if (indent > 0) res.append(' ');

            res.append(value.dump_impl(indent, depth + 1));
            first = false;
        }

        res.append('\n').append(util::String(indent * (depth - 1))).append('}');
        return res.build();
    }

private:
    i8 json_type_;
    void* json_item_;
};

template <typename T>
fn constexpr make_int(T&& value)->JsonType::JsonInt {
    using RT = std::remove_cvref_t<T>;

    if constexpr (std::is_arithmetic_v<RT>) {
        return static_cast<JsonType::JsonInt>(value);
    }

    throw runtime_exception("unsupported {} for make int", SRC_LOC, dtype(T));
}

template <typename T>
fn constexpr make_float(T&& value)->JsonType::JsonFloat {
    using RT = std::remove_cvref_t<T>;

    if constexpr (std::is_arithmetic_v<RT>) {
        return static_cast<JsonType::JsonFloat>(value);
    }

    throw runtime_exception("unsupported {} for make float", SRC_LOC, dtype(T));
}

template <typename T>
fn constexpr make_bool(T&& value)->JsonType::JsonBool {
    return static_cast<JsonType::JsonBool>(value);
}

} // namespace my::io

#endif // JSON_HPP