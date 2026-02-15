#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include "json.hpp"
#include "my_exception.hpp"

namespace my::json {

class JsonParser : public Object<JsonParser> {
public:
    static Json parse(const util::String& json_str) {
        JsonParser parser(json_str);
        Json value = parser.parse_value();
        parser.skip_ws();
        if (!parser.eof()) {
            throw runtime_exception("Unexpected trailing characters in json");
        }
        return value;
    }

private:
    explicit JsonParser(const util::String& json_str) :
            buf_(json_str.into_string()), pos_(0) {}

    bool eof() const { return pos_ >= buf_.size(); }

    char peek() const { return eof() ? '\0' : buf_[pos_]; }

    char get() { return eof() ? '\0' : buf_[pos_++]; }

    void skip_ws() {
        while (!eof() && std::isspace(static_cast<unsigned char>(peek()))) {
            ++pos_;
        }
    }

    Json parse_value() {
        skip_ws();
        if (eof()) {
            throw runtime_exception("Empty json input");
        }
        char c = peek();
        if (c == '{') return parse_object();
        if (c == '[') return parse_array();
        if (c == '"') return Json(parse_string());
        if (c == 't') return parse_true();
        if (c == 'f') return parse_false();
        if (c == 'n') return parse_null();
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parse_number();
        throw runtime_exception("Invalid json value");
    }

    Json parse_object() {
        expect('{');
        skip_ws();
        Json::Map obj;
        if (peek() == '}') {
            get();
            return Json(std::move(obj));
        }
        while (true) {
            skip_ws();
            if (peek() != '"') {
                throw runtime_exception("Expected string key in object");
            }
            util::String key = parse_string();
            skip_ws();
            expect(':');
            Json value = parse_value();
            obj.insert(std::move(key), std::move(value));
            skip_ws();
            if (peek() == ',') {
                get();
                continue;
            }
            if (peek() == '}') {
                get();
                break;
            }
            throw runtime_exception("Expected ',' or '}}' in object");
        }
        return Json(std::move(obj));
    }

    Json parse_array() {
        expect('[');
        skip_ws();
        Json::Array arr;
        if (peek() == ']') {
            get();
            return Json(std::move(arr));
        }
        while (true) {
            Json value = parse_value();
            arr.push(std::move(value));
            skip_ws();
            if (peek() == ',') {
                get();
                continue;
            }
            if (peek() == ']') {
                get();
                break;
            }
            throw runtime_exception("Expected ',' or ']' in array");
        }
        return Json(std::move(arr));
    }

    util::String parse_string() {
        expect('"');
        std::string out;
        while (!eof()) {
            char c = get();
            if (c == '"') {
                return util::String(out.c_str(), out.size());
            }
            if (c == '\\') {
                if (eof()) throw runtime_exception("Invalid escape in string");
                char esc = get();
                switch (esc) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                case 'u': append_unicode(out); break;
                default: throw runtime_exception("Invalid escape in string");
                }
            } else {
                out.push_back(c);
            }
        }
        throw runtime_exception("Unterminated string");
    }

    Json parse_true() {
        expect('t');
        expect('r');
        expect('u');
        expect('e');
        return Json(true);
    }

    Json parse_false() {
        expect('f');
        expect('a');
        expect('l');
        expect('s');
        expect('e');
        return Json(false);
    }

    Json parse_null() {
        expect('n');
        expect('u');
        expect('l');
        expect('l');
        return Json(Null{});
    }

    Json parse_number() {
        usize start = pos_;
        if (peek() == '-') {
            get();
        }
        if (peek() == '0') {
            get();
        } else {
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                throw runtime_exception("Invalid number");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                get();
            }
        }
        bool is_float = false;
        if (peek() == '.') {
            is_float = true;
            get();
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                throw runtime_exception("Invalid number");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                get();
            }
        }
        if (peek() == 'e' || peek() == 'E') {
            is_float = true;
            get();
            if (peek() == '+' || peek() == '-') {
                get();
            }
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                throw runtime_exception("Invalid number");
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                get();
            }
        }
        auto token = buf_.substr(start, pos_ - start);
        if (is_float) {
            char* end = nullptr;
            double val = std::strtod(token.c_str(), &end);
            if (!end || *end != '\0') {
                throw runtime_exception("Invalid number");
            }
            return Json(static_cast<f64>(val));
        }
        char* end = nullptr;
        long long val = std::strtoll(token.c_str(), &end, 10);
        if (!end || *end != '\0') {
            throw runtime_exception("Invalid number");
        }
        return Json(static_cast<i64>(val));
    }

    void expect(char expected) {
        if (eof() || get() != expected) {
            throw runtime_exception("Unexpected character in json");
        }
    }

    static int hex_val(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    void append_utf8(std::string& out, uint32_t codepoint) {
        if (codepoint <= 0x7F) {
            out.push_back(static_cast<char>(codepoint));
        } else if (codepoint <= 0x7FF) {
            out.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else if (codepoint <= 0xFFFF) {
            out.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
    }

    void append_unicode(std::string& out) {
        uint32_t codepoint = 0;
        for (int i = 0; i < 4; ++i) {
            if (eof()) throw runtime_exception("Invalid unicode escape");
            int v = hex_val(get());
            if (v < 0) throw runtime_exception("Invalid unicode escape");
            codepoint = (codepoint << 4) | static_cast<uint32_t>(v);
        }
        if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
            if (eof() || get() != '\\' || eof() || get() != 'u') {
                throw runtime_exception("Invalid surrogate pair");
            }
            uint32_t low = 0;
            for (int i = 0; i < 4; ++i) {
                if (eof()) throw runtime_exception("Invalid unicode escape");
                int v = hex_val(get());
                if (v < 0) throw runtime_exception("Invalid unicode escape");
                low = (low << 4) | static_cast<uint32_t>(v);
            }
            if (low < 0xDC00 || low > 0xDFFF) {
                throw runtime_exception("Invalid surrogate pair");
            }
            codepoint = 0x10000 + (((codepoint - 0xD800) << 10) | (low - 0xDC00));
        }
        append_utf8(out, codepoint);
    }

private:
    std::string buf_;
    usize pos_;
};

inline Json parse_json(const util::String& json_str) {
    return JsonParser::parse(json_str);
}

} // namespace my::json

#endif // JSON_PARSER_HPP
