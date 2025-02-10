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

template <Hashable K>
def my_hash_impl(const K& key, std::true_type)->hash_t {
    return key.__hash__();
}

template <Hashable K>
def my_hash_impl(const K& key, std::false_type)->hash_t {
    return std::hash<K>{}(key);
}

template <Hashable K>
def my_hash(const K& key)->hash_t {
    return my_hash_impl(key, std::bool_constant<MyLikeHashable<K>>{});
}

/**
 * @brief 从给定的字符指针处解码一个32位固定长度的无符号整数
 *
 * 该函数假定输入数据是以小端字节序（little-endian byte order）存储的
 * 即最低有效字节（Least Significant Byte, LSB）存储在最低的内存地址
 * 函数会将这四个字节组合成一个32位的无符号整数并返回
 *
 * @param ptr 指向包含32位无符号整数的字符数组的指针
 *            该数组至少应有4个字节的长度
 * @return 解码后的32位无符号整数
 */
def decode_fixed32(const char* ptr)->u32 {
    const u8* buffer = reinterpret_cast<const u8*>(ptr);
    return (static_cast<u32>(buffer[0]) | (static_cast<u32>(buffer[1]) << 8) | (static_cast<u32>(buffer[2]) << 16) | (static_cast<u32>(buffer[3]) << 24));
}

/**
 * @brief 字符串哈希
 *
 * 该函数使用一种基于FNV-1a哈希算法的变体来计算给定字节数据的哈希值
 * 它接受一个指向数据的指针、数据的长度以及一个可选的种子值作为输入
 * 并返回一个哈希值
 *
 * 算法细节：
 * 1. 初始化哈希值`h`为种子值`seed`与数据长度`n`乘以一个魔数`m`的异或结果
 * 2. 以4字节为单位处理数据，直到剩余数据不足4字节
 *    - 使用`decode_fixed32`函数将4字节数据解码为一个32位无符号整数`w`
 *    - 更新哈希值`h`为`(h + w) * m`，并对结果进行右移16位异或操作以减少碰撞
 * 3. 处理剩余不足4字节的数据
 *    - 将剩余字节逐个添加到哈希值`h`中，每个字节根据其位置左移相应的位数
 * 4. 最后，对哈希值`h`再次乘以魔数`m`，并进行右移`r`位异或操作，得到最终的哈希值
 *
 * @param data 指向要计算哈希值的字节数据的指针
 * @param n 数据的长度（以字节为单位）
 * @param seed 可选的种子值，用于初始化哈希计算。默认为0xbc9f1d34
 * @return 计算得到的哈希值
 */
def bytes_hash(const char* data, size_t n, u32 seed = 0xbc9f1d34)->hash_t {
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

    i32 dis = end - data;
    for (i32 i = 0; i < dis; i++) {
        h += static_cast<u8>(data[i]) << (i * 8);
    }

    h *= m;
    h ^= (h >> r);
    return h;
}

} // namespace my

#endif // HASH_HPP