/**
 * @brief hash工具
 * @author Ricky
 * @date 2024/11/27
 * @version 1.0
 */
#ifndef HASH_HPP
#define HASH_HPP

#include "ricky_concepts.hpp"

namespace my {

/**
 * @brief 计算哈希值
 * @details 使用类型实现的__hash__计算
 * @tparam K 键类型
 * @param key 键值
 * @return 哈希值
 */
template <Hashable K>
fn my_hash_impl(const K& key, std::true_type) -> hash_t {
    return key.__hash__();
}

/**
 * @brief 计算哈希值
 * @details 对于不支持__hash__的类型，使用std::hash进行计算
 * @tparam K 键类型
 * @param key 键值
 * @return 哈希值
 */
template <Hashable K>
fn my_hash_impl(const K& key, std::false_type) -> hash_t {
    return std::hash<K>{}(key);
}

/**
 * @brief 计算哈希值
 * @details 根据类型特性选择合适的哈希计算方式
 * @tparam K 键类型
 * @param key 键值
 * @return 哈希值
 */
template <Hashable K>
fn my_hash(const K& key) -> hash_t {
    return my_hash_impl(key, std::bool_constant<MyLikeHashable<K>>{});
}

/**
 * @brief 从字节数据中解码一个32位无符号整数
 * @details 假设数据是以小端字节序存储的，函数会将这四个字节组合成一个32位的无符号整数并返回
 * @param ptr 指向字节数据的指针，该数组至少应有4个字节的长度
 * @return 解码后的32位无符号整数
 */
fn decode_fixed32(const char* ptr) -> u32 {
    const auto buffer = reinterpret_cast<const u8*>(ptr);
    return (static_cast<u32>(buffer[0]) | (static_cast<u32>(buffer[1]) << 8) | (static_cast<u32>(buffer[2]) << 16) | (static_cast<u32>(buffer[3]) << 24));
}

/**
 * @brief 计算字节数据的哈希值
 * @details 使用FNV-1a变体算法，支持任意长度的字节数据
 * @note 该函数假设数据是以小端字节序存储的
 * @param data 指向字节数据的指针
 * @param n 数据长度（以字节为单位）
 * @param seed 可选的种子值，默认为0xbc9f1d34
 * @return 计算得到的哈希值
 */
fn bytes_hash(const char* data, const size_t n, const u32 seed = 0xbc9f1d34) -> hash_t {
    constexpr hash_t m = 0xc6a4a793;
    constexpr hash_t r = 24;
    const char* end = data + n;
    hash_t h = seed ^ (n * m);

    while (data + 4 < end) {
        hash_t w = decode_fixed32(data);
        data += 4;
        h = (h + w) * m;
        h ^= (h >> 16);
    }

    const auto dis = end - data;
    for (i64 i = 0; i < dis; i++) {
        h += static_cast<u8>(data[i]) << (i * 8);
    }

    h *= m;
    h ^= (h >> r);
    return h;
}

} // namespace my

#endif // HASH_HPP