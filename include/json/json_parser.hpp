#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include "json.hpp"

namespace my::json {

class JsonParser : public Object<JsonParser> {
public:
    using JsonString = str::String<>;
    using JsonStringView = str::StringView;

    static Json parse(const JsonStringView json_str) {
        JsonParser parser(json_str);
        Json value = parser.parse_value();
        parser.skip_ws();
        if (!parser.eof()) {
            throw runtime_exception("Unexpected trailing characters in json");
        }
        return value;
    }

    static Json parse(const JsonString& json_str) {
        return parse(json_str.as_str());
    }

    static Json parse(const char* json_str) {
        return parse(JsonStringView(json_str));
    }

    static Json parse(const std::string_view json_str) {
        return parse(JsonStringView(json_str.data(), json_str.size()));
    }

private:
    explicit JsonParser(const JsonStringView json_str) :
            input_(json_str), pos_(0) {}

    bool eof() const {
        return pos_ >= input_.len();
    }

    char peek() const {
        if (eof()) return '\0';
        return static_cast<char>(input_.as_bytes()[pos_]);
    }

    char get() {
        if (eof()) return '\0';
        return static_cast<char>(input_.as_bytes()[pos_++]);
    }

    void skip_ws() {
        while (!eof() && std::isspace(static_cast<unsigned char>(peek())) != 0) {
            ++pos_;
        }
    }

    Json parse_value() {
        skip_ws();
        if (eof()) {
            throw runtime_exception("Empty json input");
        }
        const char c = peek();
        if (c == '{') return parse_object();
        if (c == '[') return parse_array();
        if (c == '"') return Json(parse_string());
        if (c == 't') return parse_true();
        if (c == 'f') return parse_false();
        if (c == 'n') return parse_null();
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c)) != 0) return parse_number();
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
            Json::String key = parse_string();
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
            throw runtime_exception("Expected comma or object end");
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

    Json::String parse_string() {
        expect('"');
        Json::String out;
        usize chunk_start = pos_;

        while (!eof()) {
            const char c = peek();
            if (c == '"') {
                append_raw_chunk(out, chunk_start, pos_ - chunk_start);
                get();
                return out;
            }
            if (c == '\\') {
                append_raw_chunk(out, chunk_start, pos_ - chunk_start);
                get();
                if (eof()) throw runtime_exception("Invalid escape in string");
                const char esc = get();
                switch (esc) {
                case '"': out.push(static_cast<char32_t>('"')); break;
                case '\\': out.push(static_cast<char32_t>('\\')); break;
                case '/': out.push(static_cast<char32_t>('/')); break;
                case 'b': out.push(static_cast<char32_t>('\b')); break;
                case 'f': out.push(static_cast<char32_t>('\f')); break;
                case 'n': out.push(static_cast<char32_t>('\n')); break;
                case 'r': out.push(static_cast<char32_t>('\r')); break;
                case 't': out.push(static_cast<char32_t>('\t')); break;
                case 'u': append_unicode(out); break;
                default: throw runtime_exception("Invalid escape in string");
                }
                chunk_start = pos_;
                continue;
            }
            ++pos_;
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
        const usize start = pos_;
        if (peek() == '-') {
            get();
        }
        if (peek() == '0') {
            get();
        } else {
            if (std::isdigit(static_cast<unsigned char>(peek())) == 0) {
                throw runtime_exception("Invalid number");
            }
            while (std::isdigit(static_cast<unsigned char>(peek())) != 0) {
                get();
            }
        }

        bool is_float = false;
        if (peek() == '.') {
            is_float = true;
            get();
            if (std::isdigit(static_cast<unsigned char>(peek())) == 0) {
                throw runtime_exception("Invalid number");
            }
            while (std::isdigit(static_cast<unsigned char>(peek())) != 0) {
                get();
            }
        }

        if (peek() == 'e' || peek() == 'E') {
            is_float = true;
            get();
            if (peek() == '+' || peek() == '-') {
                get();
            }
            if (std::isdigit(static_cast<unsigned char>(peek())) == 0) {
                throw runtime_exception("Invalid number");
            }
            while (std::isdigit(static_cast<unsigned char>(peek())) != 0) {
                get();
            }
        }

        const usize token_len = pos_ - start;
        const auto* token_ptr = reinterpret_cast<const char*>(input_.as_bytes() + start);
        const std::string token(token_ptr, token_len);

        if (is_float) {
            char* end = nullptr;
            const double val = std::strtod(token.c_str(), &end);
            if (!end || *end != '\0') {
                throw runtime_exception("Invalid number");
            }
            return Json(static_cast<f64>(val));
        }

        char* end = nullptr;
        const long long val = std::strtoll(token.c_str(), &end, 10);
        if (!end || *end != '\0') {
            throw runtime_exception("Invalid number");
        }
        return Json(static_cast<i64>(val));
    }

    void expect(const char expected) {
        if (eof() || get() != expected) {
            throw runtime_exception("Unexpected character in json");
        }
    }

    static int hex_val(const char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    void append_raw_chunk(Json::String& out, const usize start, const usize len) const {
        if (len == 0) return;
        out.push_str(str::StringView(input_.as_bytes() + start, len));
    }

    void append_unicode(Json::String& out) {
        u32 codepoint = 0;
        for (int i = 0; i < 4; ++i) {
            if (eof()) throw runtime_exception("Invalid unicode escape");
            const int v = hex_val(get());
            if (v < 0) throw runtime_exception("Invalid unicode escape");
            codepoint = (codepoint << 4) | static_cast<u32>(v);
        }

        if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
            if (eof() || get() != '\\' || eof() || get() != 'u') {
                throw runtime_exception("Invalid surrogate pair");
            }

            u32 low = 0;
            for (int i = 0; i < 4; ++i) {
                if (eof()) throw runtime_exception("Invalid unicode escape");
                const int v = hex_val(get());
                if (v < 0) throw runtime_exception("Invalid unicode escape");
                low = (low << 4) | static_cast<u32>(v);
            }

            if (low < 0xDC00 || low > 0xDFFF) {
                throw runtime_exception("Invalid surrogate pair");
            }

            codepoint = 0x10000 + (((codepoint - 0xD800) << 10) | (low - 0xDC00));
        }

        out.push(static_cast<char32_t>(codepoint));
    }

private:
    JsonStringView input_;
    usize pos_;
};

inline Json parse_json(const str::StringView json_str) {
    return JsonParser::parse(json_str);
}

inline Json parse_json(const str::String<>& json_str) {
    return JsonParser::parse(json_str);
}

inline Json parse_json(const char* json_str) {
    return JsonParser::parse(json_str);
}

inline Json parse_json(const std::string_view json_str) {
    return JsonParser::parse(json_str);
}

} // namespace my::json

#endif // JSON_PARSER_HPP
