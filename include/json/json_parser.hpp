/**
* @brief Json解析工具
* @author Ricky
* @date 2025/1/11
* @version 1.0
*/
#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include "json.hpp"

namespace my::json {

class JsonParser : public Object<JsonParser> {
public:
   /**
    * @brief 解析json字符串为json对象
    * @param jsonStr json字符串，引用传递会做修剪处理
    * @return json对象
    */
   static auto parse(JsonType::JsonStr& jsonStr) -> Json {
       jsonStr = jsonStr.trim();

       if (jsonStr[0] == util::CodePoint{'{'}) {
           JsonType::JsonStr match = jsonStr.match('{', '}'); // hash_map
           return parseMap(match);
       } else if (jsonStr[0] == util::CodePoint{'['}) {
           JsonType::JsonStr match = jsonStr.match('[', ']'); // array
           return parseArray(match);
       } else if (jsonStr[0] == util::CodePoint{'\"'}) {
           return parseStr(jsonStr); // str
       } else {
           return parseSimple(jsonStr); // simple
       }
   }

   /**
    * @brief 解析json对象为json字符串
    * @param json json对象
    * @return json字符串
    */
   static auto parse(const Json& json) -> JsonType::JsonStr {
       return util::String{json.__str__()};
   }

private:
   /**
    * @brief 递归提取json字符串中的第一个JSON对象
    * @param jsonStr json字符串
    * @param stopSign 停止字符
    * @return 解析后的JSON对象和剩余的字符串
    */
   static auto parseFirstObject(JsonType::JsonStr& jsonStr, const util::CodePoint<>& stopSign) -> Pair<Json, JsonType::JsonStr> {
       jsonStr = jsonStr.trim();
       if (jsonStr.size() == 0) {
           throw argument_exception("Json string is is_empty");
       }

       JsonType::JsonStr match;
       if (jsonStr[0] == util::CodePoint{'['}) {
           match = jsonStr.match('[', ']');
       } else if (jsonStr[0] == util::CodePoint('{')) {
           match = jsonStr.match('{', '}');
       } else if (jsonStr[0] == util::CodePoint{'\"'}) {
           match = jsonStr.slice(0, jsonStr.slice(1).find(util::CodePoint{'\"'}) + 2);
       } else {
           auto stopSignIdx = jsonStr.find(stopSign);
           if (stopSignIdx == npos) {
               match = jsonStr;
           } else {
               match = jsonStr.slice(0, stopSignIdx);
           }
       }

       JsonType::JsonStr remain = jsonStr.slice(match.size()).trim();
       if (remain.size()) {
           if (remain[0] != stopSign) {
               throw not_found_exception("Stop sign \'{}\' not found", stopSign.data());
           }
           remain = remain.slice(1).trim();
       }
       return {parse(match), remain};
   }

   static auto parseMap(JsonType::JsonStr& jsonStr) -> Json {
       JsonType::JsonMap hash_map;
       jsonStr = jsonStr.slice(1, -1).trim(); // 去掉 {}

       while (jsonStr.size()) {
           auto [key, remain] = parseFirstObject(jsonStr, util::CodePoint{':'});
           jsonStr = remain;

           auto [value, remain2] = parseFirstObject(jsonStr, util::CodePoint{','});
           jsonStr = remain2;

           hash_map[key.into<JsonType::JsonStr>()] = std::move(value);
       }
       return Json{std::move(hash_map)};
   }

   static auto parseArray(JsonType::JsonStr& jsonStr) -> Json {
       JsonType::JsonArray arr;
       jsonStr = jsonStr.slice(1, -1).trim(); // 去掉 []

       while (jsonStr.size()) {
           auto [item, remain] = parseFirstObject(jsonStr, util::CodePoint{','});
           arr.push(item);
           jsonStr = remain;
       }
       return Json{std::move(arr)};
   }

   static auto parseStr(JsonType::JsonStr& jsonStr) -> Json {
       return Json{jsonStr.slice(1, -1).to_string()}; // 去掉 ""
   }

   static auto parseNum(JsonType::JsonStr& jsonStr) -> Json {
       bool isFloat = false;
       for (auto c : jsonStr) {
           if (!c.is_digit()) {
               if (c == util::CodePoint{'.'} && !isFloat) {
                   isFloat = true;
               } else {
                   throw runtime_exception("Invalid number parse: {}", jsonStr);
               }
           }
       }
       auto* str = stdstr(cstr(jsonStr));
       return isFloat ? Json{make_float(atof(str))} : Json{make_int(atoll(str))};
   }

   static auto parseNull(JsonType::JsonStr& jsonStr) -> Json {
       if (jsonStr != JsonType::JsonStr{"null", 4}) {
           throw runtime_exception("Invalid null parse: {}", jsonStr);
       }
       return Json{JsonType::JsonNull{}};
   }

   static auto parseBool(JsonType::JsonStr& jsonStr) -> Json {
       if (jsonStr == "true"_s) {
           return Json{true};
       } else if (jsonStr == "false"_s) {
           return Json{false};
       } else {
           throw runtime_exception("Invalid bool parse: {}", jsonStr);
       }
   }

   static auto parseSimple(JsonType::JsonStr& jsonStr) -> Json {
       if (jsonStr[0].is_digit()) {
           return parseNum(jsonStr); // num
       } else if (jsonStr[0] == util::CodePoint{'n'}) {
           return parseNull(jsonStr); // null
       } else if (jsonStr[0] == util::CodePoint{'t'} || jsonStr[0] == util::CodePoint{'f'}) {
           return parseBool(jsonStr); // bool
       } else {
           throw runtime_exception("Invalid simple parse: {}", jsonStr);
       }
   }
};

inline auto parse_json(JsonType::JsonStr& jsonStr) -> Json {
    return JsonParser().parse(jsonStr);
}

inline auto parse_json(const Json& json) -> JsonType::JsonStr {
    return JsonParser().parse(json);
}

} // namespace my::json

#endif // JSON_PARSER_HPP