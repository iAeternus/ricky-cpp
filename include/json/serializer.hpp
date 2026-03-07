/**
 * @brief 序列化基础接口
 * @author Ricky
 * @date 2025/7/20
 * @version 1.0
 */
#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "string.hpp"

namespace my::json {

/**
 * @brief 序列化CRTP接口
 * @note 派生类需要实现 do_serialize 和 do_deserialize
 */
template <typename D>
class Serializer : public Object<Serializer<D>> {
public:
    using Self = Serializer<D>;
    using JsonString = str::String<>;
    using JsonStringView = str::StringView;

    /**
     * @brief 序列化对象
     * @param obj 待序列化对象
     * @return 序列化后的字符串
     */
    template <typename T>
    auto serialize(const T& obj) const -> JsonString {
        return static_cast<const D*>(this)->do_serialize(obj);
    }

    /**
     * @brief 从字符串反序列化
     * @param str 输入字符串
     * @return 反序列化后的对象
     */
    template <typename T>
    auto deserialize(const JsonStringView str) const -> T {
        T res;
        static_cast<const D*>(this)->do_deserialize(str, res);
        return res;
    }

    /**
     * @brief 从字符串反序列化到已有对象
     * @param str 输入字符串
     * @param res 输出对象
     */
    template <typename T>
    auto deserialize(const JsonStringView str, T& res) const -> void {
        static_cast<const D*>(this)->do_deserialize(str, res);
    }

    template <typename T>
    auto deserialize(const JsonString& str) const -> T {
        return deserialize<T>(str.as_str());
    }

    template <typename T>
    auto deserialize(const char* str) const -> T {
        return deserialize<T>(JsonStringView(str));
    }

    template <typename T>
    auto deserialize(const std::string_view str) const -> T {
        return deserialize<T>(JsonStringView(str.data(), str.size()));
    }

    template <typename T>
    auto deserialize(const char* str, T& res) const -> void {
        deserialize(JsonStringView(str), res);
    }

    template <typename T>
    auto deserialize(const std::string_view str, T& res) const -> void {
        deserialize(JsonStringView(str.data(), str.size()), res);
    }
};

} // namespace my::json

#endif // SERIALIZER_HPP
