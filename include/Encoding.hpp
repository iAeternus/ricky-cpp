/**
 * @brief 字符编码
 * @author Ricky
 * @date 2025/7/14
 * @version 2.0
 */
#ifndef ENCODING_HPP
#define ENCODING_HPP

#include "Dict.hpp"
#include <functional>

namespace tmp {

using namespace my;

/**
 * @brief 字符编码类型
 */
enum class Encoding {
    ASCII,
    UTF8,
    UTF16,
    UTF32,
    GB2312,
    LATIN1
};

/**
 * @brief 字符串编码类型转换
 */
class EncodingConv : public Object<EncodingConv> {
public: 
    using Self = EncodingConv;
    // using Converter = std::function<class Fty>
};

}; // namespace tmp

#endif // ENCODING_HPP