/**
 * @brief 序列化工具
 * @author Ricky
 * @date 2025/7/20
 * @version 1.0
 */
#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "str.hpp"

namespace my::json {

/**
 * @brief 序列化CRTP接口
 * @note 派生类需要实现 do_serialize 和 do_deserialize
 */
template <typename D>
class Serializer : public Object<Serializer<D>> {
public:
    using Self = Serializer<D>;

    /**
     * @brief 序列化对象
     * @details 该函数将对象obj序列化为一个字符串
     * @param obj 序列化的对象
     * @return 序列化后的字符串
     */
    template <typename T>
    auto serialize(const T& obj) const -> util::String {
        return static_cast<const D*>(this)->do_serialize(obj);
    }

    /**
     * @brief 反序列化对象
     * @details 该函数将str反序列化为一个对象
     * @param str 反序列化的字符串
     * @return 反序列化后的对象
     */
    template <typename T>
    auto deserialize(const util::String& str) const -> T {
        T res;
        static_cast<const D*>(this)->do_deserialize(str, res);
        return res;
    }

    /**
     * @brief 反序列化到现有对象
     * @param str 反序列化的字符串
     * @param res 反序列化后的对象
     */
    template <typename T>
    auto deserialize(const util::String& str, T& res) const -> void {
        static_cast<const D*>(this)->do_deserialize(str, res);
    }
};

} // namespace my::json

#endif // SERIALIZER_HPP