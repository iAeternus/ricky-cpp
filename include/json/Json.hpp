/**
 * @brief Json对象
 * @author Ricky
 * @date 2025/1/11
 * @version 1.0
 */
#ifndef JSON_HPP
#define JSON_HPP

#include "json_trait.hpp"

namespace my::json {

class Json : public Object<Json> {
    using Self = Json;

public:
    Json() :
            Json(Null{}) {}

    template <JsonTypeStrictConcept T>
    Json(T&& item) :
            jsonType_(GetJsonTypeIDStrict<T>::ID), jsonItem_(new T(std::forward<T>(item))) {}

    Json(const Self& other) :
            jsonType_(other.jsonType_) {
        switch (jsonType_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            this->jsonItem_ = new JsonType::JsonInt(other.transform<JsonType::JsonInt>());
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            this->jsonItem_ = new JsonType::JsonFloat(other.transform<JsonType::JsonFloat>());
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            this->jsonItem_ = new JsonType::JsonBool(other.transform<JsonType::JsonBool>());
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            this->jsonItem_ = new JsonType::JsonStr(other.transform<JsonType::JsonStr>());
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            this->jsonItem_ = new JsonType::JsonArray(other.transform<JsonType::JsonArray>());
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            this->jsonItem_ = new JsonType::JsonDict(other.transform<JsonType::JsonDict>());
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            this->jsonItem_ = new JsonType::JsonNull(other.transform<JsonType::JsonNull>());
            break;
        default:
            ValueError(std::format("json type {} is not valid", jsonType_));
        }
    }

    Json(Self&& other) noexcept :
            jsonType_(other.jsonType_), jsonItem_(other.jsonItem_) {
        other.jsonType_ = GetJsonTypeIDStrict<JsonType::JsonNull>::ID;
        other.jsonItem_ = nullptr;
    }

    ~Json() {
        release();
    }

    Self& operator=(const Self& other) {
        if (this == &other) return *this;

        release();
        jsonType_ = other.jsonType_;

        switch (jsonType_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            this->jsonItem_ = new JsonType::JsonInt(other.transform<JsonType::JsonInt>());
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            this->jsonItem_ = new JsonType::JsonFloat(other.transform<JsonType::JsonFloat>());
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            this->jsonItem_ = new JsonType::JsonBool(other.transform<JsonType::JsonBool>());
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            this->jsonItem_ = new JsonType::JsonStr(other.transform<JsonType::JsonStr>());
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            this->jsonItem_ = new JsonType::JsonArray(other.transform<JsonType::JsonArray>());
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            this->jsonItem_ = new JsonType::JsonDict(other.transform<JsonType::JsonDict>());
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            this->jsonItem_ = new JsonType::JsonNull(other.transform<JsonType::JsonNull>());
            break;
        default:
            ValueError(std::format("json type {} is not valid", jsonType_));
        }
        return *this;
    }

    Self& operator=(Self&& other) noexcept {
        if (this == &other) return *this;

        release();
        this->jsonType_ = other.jsonType_;
        this->jsonItem_ = other.jsonItem_;

        other.jsonType_ = GetJsonTypeID<JsonType::JsonNull>::ID;
        other.jsonItem_ = nullptr;
        return *this;
    }

    void swap(Self& other) noexcept {
        std::swap(this->jsonType_, other.jsonType_);
        std::swap(this->jsonItem_, other.jsonItem_);
    }

    template <JsonTypeStrictConcept T>
    bool is() const {
        return jsonType_ == GetJsonTypeID<T>::ID;
    }

    /**
     * @brief 返回Json存储的对象类型名
     */
    static CString typeName(i8 id) {
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
            ValueError(std::format("json type {} is not valid", id));
            std::unreachable();
        }
    }

    /**
     * @brief 转换为指定类型
     * @return 返回转换后对象的指针
     */
    template <JsonTypeStrictConcept T>
    T* transformPtr() {
        if (!is<T>()) {
            ValueError(std::format("Json type is not {} but {}", typeName(GetJsonTypeID<T>::ID), typeName(jsonType_)));
        }
        return reinterpret_cast<T*>(jsonItem_);
    }

    template <JsonTypeStrictConcept T>
    const T* transformPtr() const {
        if (!is<T>()) {
            ValueError(std::format("Json type is not {} but {}", typeName(GetJsonTypeID<T>::ID), typeName(jsonType_)));
        }
        return reinterpret_cast<T*>(jsonItem_);
    }

    /**
     * @brief 转换为指定类型
     * @return 返回转换后对象的引用
     */
    template <JsonTypeStrictConcept T>
    T& transform() {
        return *transformPtr<T>();
    }

    template <JsonTypeStrictConcept T>
    const T& transform() const {
        return *transformPtr<T>();
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
        switch (jsonType_) {
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
            ValueError(std::format("json type {} is not valid", jsonType_));
            std::unreachable();
        }
    }

private:
    /**
     * @brief 释放内存
     */
    void release() {
        switch (jsonType_) {
        case GetJsonTypeID<JsonType::JsonInt>::ID:
            delete transformPtr<JsonType::JsonInt>();
            break;
        case GetJsonTypeID<JsonType::JsonFloat>::ID:
            delete transformPtr<JsonType::JsonFloat>();
            break;
        case GetJsonTypeID<JsonType::JsonBool>::ID:
            delete transformPtr<JsonType::JsonBool>();
            break;
        case GetJsonTypeID<JsonType::JsonStr>::ID:
            delete transformPtr<JsonType::JsonStr>();
            break;
        case GetJsonTypeID<JsonType::JsonArray>::ID:
            delete transformPtr<JsonType::JsonArray>();
            break;
        case GetJsonTypeID<JsonType::JsonDict>::ID:
            delete transformPtr<JsonType::JsonDict>();
            break;
        case GetJsonTypeID<JsonType::JsonNull>::ID:
            delete transformPtr<JsonType::JsonNull>();
            break;
        default:
            ValueError(std::format("Json type {} is not valid", jsonType_));
        }
    }

private:
    i8 jsonType_;
    void* jsonItem_;
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