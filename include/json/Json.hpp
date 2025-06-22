/**
 * @brief Json对象
 * @author Ricky
 * @date 2025/1/11
 * @version 1.0
 */
#ifndef JSON_HPP
#define JSON_HPP

#include "Exception.hpp"
#include "json_trait.hpp"

namespace my::json {

class Json : public Object<Json> {
public:
    using Self = Json;

    Json() :
            Json(Null{}) {}

    template <JsonTypeStrictConcept T>
    Json(T&& item) :
            json_type_(GetJsonTypeIDStrict<T>::ID), json_item_(new T(std::forward<T>(item))) {}

    Json(const Self& other) :
            json_type_(other.json_type_) {
        switch (json_type_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            this->json_item_ = new JsonType::JsonInt(other.transform<JsonType::JsonInt>());
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            this->json_item_ = new JsonType::JsonFloat(other.transform<JsonType::JsonFloat>());
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            this->json_item_ = new JsonType::JsonBool(other.transform<JsonType::JsonBool>());
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            this->json_item_ = new JsonType::JsonStr(other.transform<JsonType::JsonStr>());
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            this->json_item_ = new JsonType::JsonArray(other.transform<JsonType::JsonArray>());
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            this->json_item_ = new JsonType::JsonDict(other.transform<JsonType::JsonDict>());
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            this->json_item_ = new JsonType::JsonNull(other.transform<JsonType::JsonNull>());
            break;
        default:
            type_exception("json type {} is not valid", SRC_LOC, json_type_);
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
            this->json_item_ = new JsonType::JsonInt(other.transform<JsonType::JsonInt>());
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            this->json_item_ = new JsonType::JsonFloat(other.transform<JsonType::JsonFloat>());
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            this->json_item_ = new JsonType::JsonBool(other.transform<JsonType::JsonBool>());
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            this->json_item_ = new JsonType::JsonStr(other.transform<JsonType::JsonStr>());
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            this->json_item_ = new JsonType::JsonArray(other.transform<JsonType::JsonArray>());
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            this->json_item_ = new JsonType::JsonDict(other.transform<JsonType::JsonDict>());
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            this->json_item_ = new JsonType::JsonNull(other.transform<JsonType::JsonNull>());
            break;
        default:
            type_exception("json type {} is not valid", SRC_LOC, json_type_);
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

    void swap(Self& other) noexcept {
        std::swap(this->json_type_, other.json_type_);
        std::swap(this->json_item_, other.json_item_);
    }

    template <JsonTypeStrictConcept T>
    bool is() const {
        return json_type_ == GetJsonTypeID<T>::ID;
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
            type_exception("json type {} is not valid", SRC_LOC, id);
            break;
        }
    }

    /**
     * @brief 转换为指定类型
     * @return 返回转换后对象的指针
     */
    template <JsonTypeStrictConcept T>
    T* transform_ptr() {
        if (!is<T>()) {
            type_exception("json type is not {} but {}", SRC_LOC, type_name(GetJsonTypeID<T>::ID), type_name(json_type_));
        }
        return reinterpret_cast<T*>(json_item_);
    }

    template <JsonTypeStrictConcept T>
    const T* transform_ptr() const {
        if (!is<T>()) {
            type_exception("json type is not {} but {}", SRC_LOC, type_name(GetJsonTypeID<T>::ID), type_name(json_type_));
        }
        return reinterpret_cast<T*>(json_item_);
    }

    /**
     * @brief 转换为指定类型
     * @return 返回转换后对象的引用
     */
    template <JsonTypeStrictConcept T>
    T& transform() {
        return *transform_ptr<T>();
    }

    template <JsonTypeStrictConcept T>
    const T& transform() const {
        return *transform_ptr<T>();
    }

    template <JsonTypeStrictConcept T>
    operator T() const {
        return transform<T>();
    }

    /**
     * @brief 键访问，需要当前Json对象为JsonDict类型
     */
    Self& operator[](const JsonType::JsonStr& key) {
        return transform<JsonType::JsonDict>().get(key);
    }

    const Self& operator[](const JsonType::JsonStr& key) const {
        return transform<JsonType::JsonDict>().get(key);
    }

    /**
     * @brief 尾部添加一个Json对象，需要当前Json对象为JsonArray类型
     */
    Self& append(const Self& json) {
        transform<JsonType::JsonArray>().append(json);
        return *this;
    }

    /**
     * @brief 下标访问，需要当前Json对象为JsonArray类型
     * @param index 索引，从0开始
     */
    Self& operator[](usize index) {
        return transform<JsonType::JsonArray>()[index];
    }

    const Self& operator[](usize index) const {
        return transform<JsonType::JsonArray>()[index];
    }

    /**
     * @brief 移除指定位置的Json对象，需要当前Json对象为JsonArray类型
     * @param index 索引，从0开始。若为-1，则移除最后一个元素
     * @return void
     */
    void pop(isize index = -1) {
        transform<JsonType::JsonArray>().pop(index);
    }

    /**
     * @brief 获取json对象的长度，需要当前Json对象为JsonArray或JsonDict类型
     */
    usize size() const {
        if (is<JsonType::JsonArray>()) {
            return transform<JsonType::JsonArray>().size();
        }
        if (is<JsonType::JsonDict>()) {
            return transform<JsonType::JsonDict>().size();
        }
        RuntimeError("Json type is not JSON_ARRAY or JSON_DICT");
        std::unreachable();
    }

    [[nodiscard]] CString __str__() const {
        switch (json_type_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            return cstr(transform<JsonType::JsonInt>());
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            return cstr(transform<JsonType::JsonFloat>());
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            return cstr(transform<JsonType::JsonBool>());
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            using namespace my::util;
            return cstr("\""_s + transform<JsonType::JsonStr>() + "\""_s);
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            return cstr(transform<JsonType::JsonArray>());
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            return cstr(transform<JsonType::JsonDict>());
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            return cstr("null");
        default:
            type_exception("json type {} is not valid", SRC_LOC, json_type_);
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
            delete transform_ptr<JsonType::JsonInt>();
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            delete transform_ptr<JsonType::JsonFloat>();
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            delete transform_ptr<JsonType::JsonBool>();
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            delete transform_ptr<JsonType::JsonStr>();
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            delete transform_ptr<JsonType::JsonArray>();
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            delete transform_ptr<JsonType::JsonDict>();
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            delete transform_ptr<JsonType::JsonNull>();
            break;
        default:
            type_exception("json type {} is not valid", SRC_LOC, json_type_);
        }
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

    RuntimeError(std::format("Unsupported {} for make int", dtype(T)));
    std::unreachable();
}

template <typename T>
fn constexpr make_float(T&& value)->JsonType::JsonFloat {
    using RT = std::remove_cvref_t<T>;

    if constexpr (std::is_arithmetic_v<RT>) {
        return static_cast<JsonType::JsonFloat>(value);
    }

    RuntimeError(std::format("Unsupported {} for make f32", dtype(T)));
    std::unreachable();
}

template <typename T>
fn constexpr make_bool(T&& value)->JsonType::JsonBool {
    return static_cast<JsonType::JsonBool>(value);
}

} // namespace my::json

#endif // JSON_HPP