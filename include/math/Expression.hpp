/**
 * @brief 表达式
 * @author Ricky
 * @date 2025/3/8
 * @version 1.0
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "math_utils.hpp"
#include "Vector.hpp"
#include "Dict.hpp"
// #include "Stack.hpp"

#include <stack>
#include <algorithm>

namespace my::math {

/**
 * @brief 表达式Token
 */
struct Token : public Object<Token> {
    enum Type {
        NUMBER,      // 数字
        OPERATOR,    // 二元操作符
        LEFT_PAREN,  // 左括号
        RIGHT_PAREN, // 右括号
        UNARY_OP     // 一元操作符（如负号）
    };

    struct OpProperty {
        i32 prec;        // 优先级
        bool rightAssoc; // 是否右结合
    };

    Type type;           // 标记类型
    std::string opValue; // 操作符值
    f64 numValue;        // 数字值

    Token(Type type, const std::string& value = "", f64 num = 0.0) :
            type(type), opValue(value), numValue(num) {}

    /**
     * @brief 获取操作符属性（优先级和结合性）
     *
     * @return OpProperty 操作符属性
     */
    OpProperty getOpProperty() const {
        static const util::Dict<std::string, OpProperty> opMap = {
            {"+", {2, false}}, // 加法
            {"-", {2, false}}, // 减法
            {"*", {3, false}}, // 乘法
            {"/", {3, false}}, // 除法
            {"%", {3, false}}, // 取模
            {"^", {4, true}},  // 幂运算（右结合）
            {"u-", {5, true}}  // 一元负号（右结合）
        };
        return opMap.get(opValue);
    }

    CString __str__() const {
        std::stringstream stream;
        if (opValue.empty()) {
            if (opValue == "u-") {
                stream << '-';
            } else {
                stream << numValue;
            }
        } else {
            stream << opValue;
        }
        return CString{stream.str()};
    }
};

/**
 * @brief 表达式
 */
class Expression : public Object<Expression> {
public:
    using self = Expression;

    Expression(const std::string& expr) :
            valid_(false) {
        try {
            std::string filtered;
            std::remove_copy_if(expr.begin(), expr.end(), back_inserter(filtered), [](char c) {
                return isspace(c);
            });

            tokenize(filtered);
            this->valid_ = checkBrackets();
        } catch (const std::exception& ex) {
            RuntimeError(std::format("Tokenization error: {}", std::string(ex.what())));
        }
    }

    /**
     * @brief 检查表达式是否有效
     */
    bool isValid() const {
        return valid_;
    }

    /**
     * @brief 转为后缀表达式
     */
    util::Vector<Token> toPostfix() const {
        if (!valid_) RuntimeError("Invalid expression");
        return in2post();
    }

    /**
     * @brief 计算表达式值
     */
    f64 eval() const {
        if (!valid_) RuntimeError("Invalid expression");
        return evalPostfix(in2post());
    }

    CString __str__() const {
        return tokens_.__str__();
    }

private:
    util::Vector<Token> tokens_; // 表达式标记
    bool valid_;                 // 表达式是否有效

private:
    /**
     * @brief 分词函数，将表达式分解为tokens
     */
    void tokenize(const std::string& expr) {
        tokens_.clear();
        std::string numStr;
        auto handleNumber = [&]() {
            if (numStr.empty()) return;
            checkNumber(numStr);
            tokens_.append(Token::NUMBER, "", std::stod(numStr));
            numStr.clear();
        };

        for (const auto& c : expr) {
            if (std::isdigit(c) || c == '.') {
                numStr += c;
            } else {
                handleNumber();
                if (isUnaryNegSign(c)) {
                    tokens_.append(Token::UNARY_OP, "u-");
                } else if (c == '(') {
                    tokens_.append(Token::LEFT_PAREN, "(");
                } else if (c == ')') {
                    tokens_.append(Token::RIGHT_PAREN, ")");
                } else if (isOperator(c)) {
                    tokens_.append(Token::OPERATOR, std::string(1, c));
                } else if (!std::isspace(c)) {
                    RuntimeError(std::format("Invalid character: {}", c));
                }
            }
        }
        handleNumber();
    }

    /**
     * @brief 校验数字格式
     */
    void checkNumber(const std::string& num) {
        if (num.empty()) return;

        auto dotPos = num.find('.');
        if (dotPos != std::string::npos) {
            // 还有其他小数点
            if (num.find('.', dotPos + 1) != std::string::npos) {
                RuntimeError(std::format("Invalid number (multiple dots): {}", num));
            }
            // 小数点在开头或结尾
            if (dotPos == 0 || dotPos == num.length() - 1) {
                RuntimeError(std::format("Invalid number (misplaced dot): {}", num));
            }
        }
        if (num[0] == '-' && num.size() == 1) {
            RuntimeError(std::format("Invalid operator position: {}", num));
        }
    }

    /**
     * @brief 判断是否为一元负号
     */
    bool isUnaryNegSign(char c) const {
        return c == '-' && (tokens_.empty() || tokens_.back().type == Token::LEFT_PAREN || tokens_.back().type == Token::OPERATOR || tokens_.back().type == Token::UNARY_OP);
    }

    /**
     * @brief 判断是否为操作符
     */
    bool isOperator(char c) const {
        static const std::string ops("+-*/%^");
        return ops.find(c) != std::string::npos;
    }

    /**
     * @brief 判断括号是否匹配
     */
    bool checkBrackets() const {
        std::stack<Token> st;
        for (const auto& token : tokens_) {
            if (token.type == Token::LEFT_PAREN) {
                st.push(token);
            } else if (token.type == Token::RIGHT_PAREN) {
                if (st.empty() || st.top().type != Token::LEFT_PAREN) {
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
    util::Vector<Token> in2post() const {
        util::Vector<Token> ans;
        std::stack<Token> opSt;

        for (const auto& token : tokens_) {
            switch (token.type) {
            case Token::NUMBER:
                ans.append(token);
                break;
            case Token::LEFT_PAREN:
                opSt.push(token);
                break;
            case Token::RIGHT_PAREN:
                while (!opSt.empty() && opSt.top().type != Token::LEFT_PAREN) {
                    ans.append(opSt.top());
                    opSt.pop();
                }
                if (opSt.empty()) RuntimeError("Mismatched parentheses");
                opSt.pop(); // pop左括号
                break;
            case Token::OPERATOR:
            case Token::UNARY_OP:
                while (!opSt.empty() && opSt.top().type != Token::LEFT_PAREN && shouldPop(token, opSt.top())) {
                    ans.append(opSt.top());
                    opSt.pop(); // TODO 用 util::Stack 这里段错误
                }
                opSt.push(token); // 当前操作符入栈
                break;
            default:
                RuntimeError("Unexpected token type");
            }
        }

        // 处理剩下的操作符
        while (!opSt.empty()) {
            if (opSt.top().type == Token::LEFT_PAREN) {
                RuntimeError("Mismatched parentheses");
            }
            ans.append(opSt.top());
            opSt.pop();
        }

        return ans;
    }

    /**
     * @brief 判断是否需要弹出操作符
     * @note 当遇到操作符时，对于左结合操作符，需要弹出所有优先级小于等于当前操作符的操作符；对于右操作符是小于
     */
    bool shouldPop(const Token& cur, const Token& stackTop) const {
        auto curProp = cur.getOpProperty();
        auto stackProp = stackTop.getOpProperty();

        if (curProp.rightAssoc) {
            return curProp.prec < stackProp.prec;
        } else {
            return curProp.prec <= stackProp.prec;
        }
    }

    /**
     * @brief 后缀表达式求值
     */
    f64 evalPostfix(const util::Vector<Token>& postfix) const {
        std::stack<f64> st;
        f64 a, b, x;
        for (const auto& token : postfix) {
            switch (token.type) {
            case Token::NUMBER:
                st.push(token.numValue);
                break;
            case Token::OPERATOR:
                if (st.size() < 2) RuntimeError("Insufficient operands");
                b = st.top();
                st.pop();
                a = st.top();
                st.pop();
                st.push(evalOperator(a, b, token.opValue));
                break;
            case Token::UNARY_OP:
                x = st.top();
                st.pop();
                st.push(evalUnaryOp(x, token.opValue));
                break;
            default:
                RuntimeError("Unexpected token in postfix");
            }
        }
        if (st.size() != 1) RuntimeError("Malformed expression");
        return st.top();
    }

    static f64 evalOperator(f64 a, f64 b, const std::string& op) {
        if (op == "+") {
            return a + b;
        } else if (op == "-") {
            return a - b;
        } else if (op == "*") {
            return a * b;
        } else if (op == "/") {
            if (isZero(b)) {
                RuntimeError("Divide by zero");
            }
            return a / b;
        } else if (op == "%") {
            return std::fmod(a, b);
        } else if (op == "^") {
            return std::pow(a, b);
        } else {
            ValueError(std::format("Unknown operator: {}", op));
            return None<f64>;
        }
    }

    static f64 evalUnaryOp(f64 x, const std::string& op) {
        if (op == "u-") {
            return -x;
        } else {
            ValueError(std::format("Unknown unary operator: {}", op));
            return None<f64>;
        }
    }
};

} // namespace my::math

fn operator""_expr(const char* str, size_t len)->my::math::Expression {
    return my::math::Expression(std::string(str));
}

#endif // EXPRESSION_HPP