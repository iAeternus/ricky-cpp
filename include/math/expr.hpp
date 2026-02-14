/**
 * @brief 表达式
 * @author Ricky
 * @date 2025/3/8
 * @version 1.0
 */
#ifndef EXPR_HPP
#define EXPR_HPP

#include <utility>

#include "math_utils.hpp"
#include "vec.hpp"
#include "hash_map.hpp"
#include "vec_stack.hpp"

namespace my::math {

/**
 * @brief 表达式Token
 */
struct Token : Object<Token> {
    enum Type {
        NUMBER,      // 数字
        OPERATOR,    // 二元操作符
        LEFT_PAREN,  // 左括号
        RIGHT_PAREN, // 右括号
        UNARY_OP     // 一元操作符（如负号）
    };

    struct OpProp {
        i32 prec;         // 优先级
        bool right_assoc; // 是否右结合
    };

    Type type;        // 标记类型
    CString op_value; // 操作符值
    f64 num_value;    // 数字值

    Token(Type type, CString value = "", f64 num = 0.0) :
            type(type), op_value(std::move(value)), num_value(num) {}

    /**
     * @brief 获取操作符属性（优先级和结合性）
     *
     * @return OpProp 操作符属性
     */
    OpProp get_op_prop() const {
        static const util::HashMap<CString, OpProp> op_mp = {
            {"+", {2, false}}, // 加法
            {"-", {2, false}}, // 减法
            {"*", {3, false}}, // 乘法
            {"/", {3, false}}, // 除法
            {"%", {3, false}}, // 取模
            {"^", {4, true}},  // 幂运算（右结合）
            {"u-", {5, true}}  // 一元负号（右结合）
        };
        return op_mp.get(op_value);
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        if (op_value.empty()) {
            if (op_value == "u-") {
                stream << '-';
            } else {
                stream << num_value;
            }
        } else {
            stream << op_value.data();
        }
        return CString{stream.str()};
    }
};

/**
 * @brief 表达式
 */
class Expr : public Object<Expr> {
public:
    using Self = Expr;

    Expr(const CString& expr) :
            valid_(false) {
        try {
            expr.remove_all(' ');
            tokenize(expr);
            this->valid_ = check_brackets();
        } catch (const std::exception& ex) {
            throw runtime_exception("Tokenization error: {}", ex.what());
        }
    }

    /**
     * @brief 检查表达式是否有效
     */
    bool is_valid() const {
        return valid_;
    }

    /**
     * @brief 转为后缀表达式
     */
    util::Vec<Token> to_post() const {
        if (!valid_) throw runtime_exception("Invalid expression");
        return in2post();
    }

    /**
     * @brief 计算表达式值
     */
    f64 eval() const {
        if (!valid_) throw runtime_exception("Invalid expression");
        return eval_post(in2post());
    }

    [[nodiscard]] CString __str__() const {
        return tokens_.__str__();
    }

private:
    /**
     * @brief 分词函数，将表达式分解为tokens
     */
    void tokenize(const CString& expr) {
        tokens_.clear();
        std::string num_str;
        auto handle_num = [&]() {
            if (num_str.empty()) return;
            check_num(num_str);
            tokens_.append(Token::NUMBER, "", std::stod(num_str));
            num_str.clear();
        };

        for (char c : expr) {
            if ((c >= '0' && c <= '9') || c == '.') {
                num_str += c;
            } else {
                handle_num();
                if (is_unary_neg_sign(c)) {
                    tokens_.append(Token::UNARY_OP, "u-");
                } else if (c == '(') {
                    tokens_.append(Token::LEFT_PAREN, "(");
                } else if (c == ')') {
                    tokens_.append(Token::RIGHT_PAREN, ")");
                } else if (is_op(c)) {
                    tokens_.append(Token::OPERATOR, CString::of(c));
                } else if (c != ' ') {
                    throw runtime_exception("invalid character: {}", c);
                }
            }
        }
        handle_num();
    }

    /**
     * @brief 校验数字格式
     */
    void check_num(const std::string& num) {
        if (num.empty()) return;

        auto dot_pos = num.find('.');
        if (dot_pos != std::string::npos) {
            // 还有其他小数点
            if (num.find('.', dot_pos + 1) != std::string::npos) {
                throw runtime_exception("invalid number (multiple dots): {}", num);
            }
            // 小数点在开头或结尾
            if (dot_pos == 0 || dot_pos == num.length() - 1) {
                throw runtime_exception("invalid number (misplaced dot): {}", num);
            }
        }
        if (num[0] == '-' && num.size() == 1) {
            throw runtime_exception("invalid operator position: {}", num);
        }
    }

    /**
     * @brief 判断是否为一元负号
     */
    bool is_unary_neg_sign(char ch) const {
        return ch == '-' && (tokens_.empty() || tokens_.back().type == Token::LEFT_PAREN || tokens_.back().type == Token::OPERATOR || tokens_.back().type == Token::UNARY_OP);
    }

    /**
     * @brief 判断是否为操作符
     */
    bool is_op(char ch) const {
        static const CString ops = "+-*/%^";
        return ops.find(ch) != npos;
    }

    /**
     * @brief 判断括号是否匹配
     */
    bool check_brackets() const {
        util::Stack<Token> st;
        for (const auto& token : tokens_) {
            if (token.type == Token::LEFT_PAREN) {
                st.push(token);
            } else if (token.type == Token::RIGHT_PAREN) {
                if (st.empty() || st.peek().type != Token::LEFT_PAREN) {
                    return false;
                }
                st.pop();
            }
        }
        return st.empty();
    }

    /**
     * @brief 前缀表达式转后缀表达式
     */
    util::Vec<Token> in2post() const {
        util::Vec<Token> ans;
        util::Stack<Token> op_st;

        for (const auto& token : tokens_) {
            switch (token.type) {
            case Token::NUMBER:
                ans.append(token);
                break;
            case Token::LEFT_PAREN:
                op_st.push(token);
                break;
            case Token::RIGHT_PAREN:
                while (!op_st.empty() && op_st.peek().type != Token::LEFT_PAREN) {
                    ans.append(op_st.peek());
                    op_st.pop();
                }
                if (op_st.empty()) throw runtime_exception("mismatched parentheses");
                op_st.pop(); // pop左括号
                break;
            case Token::OPERATOR:
            case Token::UNARY_OP:
                while (!op_st.empty() && op_st.peek().type != Token::LEFT_PAREN && should_pop(token, op_st.peek())) {
                    ans.append(op_st.peek());
                    op_st.pop();
                }
                op_st.push(token); // 当前操作符入栈
                break;
            default:
                throw runtime_exception("unexpected token type");
            }
        }

        // 处理剩下的操作符
        while (!op_st.empty()) {
            if (op_st.peek().type == Token::LEFT_PAREN) {
                throw runtime_exception("mismatched parentheses");
            }
            ans.append(op_st.peek());
            op_st.pop();
        }

        return ans;
    }

    /**
     * @brief 判断是否需要弹出操作符
     * @note 当遇到操作符时，对于左结合操作符，需要弹出所有优先级小于等于当前操作符的操作符；对于右操作符是小于
     */
    bool should_pop(const Token& cur, const Token& stack_top) const {
        auto cur_prop = cur.get_op_prop();
        auto top_prop = stack_top.get_op_prop();

        if (cur_prop.right_assoc) {
            return cur_prop.prec < top_prop.prec;
        } else {
            return cur_prop.prec <= top_prop.prec;
        }
    }

    /**
     * @brief 后缀表达式求值
     */
    f64 eval_post(const util::Vec<Token>& post) const {
        util::Stack<f64> st;
        f64 a, b, x;
        for (const auto& token : post) {
            switch (token.type) {
            case Token::NUMBER:
                st.push(token.num_value);
                break;
            case Token::OPERATOR:
                if (st.size() < 2) throw runtime_exception("insufficient operands");
                b = st.peek();
                st.pop();
                a = st.peek();
                st.pop();
                st.push(eval_op(a, b, token.op_value));
                break;
            case Token::UNARY_OP:
                x = st.peek();
                st.pop();
                st.push(eval_unary_op(x, token.op_value));
                break;
            default:
                throw runtime_exception("unexpected token in postfix");
            }
        }
        if (st.size() != 1) throw runtime_exception("malformed expression");
        return st.peek();
    }

    static f64 eval_op(f64 a, f64 b, const CString& op) {
        if (op == "+") {
            return a + b;
        }
        if (op == "-") {
            return a - b;
        }
        if (op == "*") {
            return a * b;
        }
        if (op == "/") {
            if (is_zero(b)) {
                throw arithmetic_exception("/ by zero");
            }
            return a / b;
        }
        if (op == "%") {
            return std::fmod(a, b);
        }
        if (op == "^") {
            return std::pow(a, b);
        }
        throw runtime_exception("unknown operator: {}", op);
    }

    static f64 eval_unary_op(f64 x, const CString& op) {
        if (op == "u-") {
            return -x;
        } else {
            throw runtime_exception("unknown unary operator: {}", op);
        }
    }

private:
    util::Vec<Token> tokens_; // 表达式标记
    bool valid_;              // 表达式是否有效
};

} // namespace my::math

namespace my {

/**
 * @brief 自定义字符串字面量，支持 `_expr` 后缀转换为 `math::Expr` 对象
 * @param str C 风格字符串
 * @param len 字符串长度
 * @return 转换后的 `math::Expr` 对象
 */
inline auto operator""_expr(const char* str, size_t len)->math::Expr {
    return math::Expr(CString(str, len));
}

} // namespace my

#endif // EXPR_HPP