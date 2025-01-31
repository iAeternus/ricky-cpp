/**
 * @brief 矩阵
 * @author Ricky
 * @date 2025/1/14
 * @version 1.0
 */
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "math_utils.hpp"
#include "DynArray.hpp"

namespace my::math {

template<typename E>
class Matrix : public Object<Matrix<E>> {
    using self = Matrix<E>;

public:
    using value_t = E;

    Matrix(c_size rows = 1, c_size cols = 1, value_t value = 0.0) :
            rows_(rows), cols_(cols), data_(rows_, util::DynArray<value_t>(cols_, value)) {}

    Matrix(std::initializer_list<std::initializer_list<value_t>>&& initList) :
            rows_(initList.size()) {
        for (auto&& row : initList) {
            data_.append(util::DynArray<value_t>{std::move(row)});
        }
        if (rows_ > 0) {
            cols_ = data_[0].size();
        }
    }

    /**
     * @brief 对角阵
     */
    Matrix(std::initializer_list<value_t>&& initList) :
            Matrix(initList.size(), initList.size()) {
        c_size pos = 0;
        for (auto&& item : initList) {
            data_[pos][pos] = item;
            ++pos;
        }
    }

    c_size rows() const {
        return rows_;
    }

    c_size cols() const {
        return cols_;
    }

    value_t& at(c_size i, c_size j) {
        return data_.at(i).at(j);
    }

    const value_t& at(c_size i, c_size j) const {
        return data_.at(i).at(j);
    }

    util::DynArray<value_t>& operator[](c_size index) {
        return data_[index];
    }

    const util::DynArray<value_t>& operator[](c_size index) const {
        return data_[index];
    }

    /**
     * @brief 获取当前矩阵的子矩阵
     */
    self getMat(c_size i1, c_size j1, c_size i2, c_size j2) const {
        i2 = neg_index(i2, rows_);
        j2 = neg_index(j2, cols_);
        if (i1 < 0 || j1 < 0 || i1 > i2 || j1 > j2) {
            ValueError(std::format("Cannot get submatrix [{}..{}] x [{}..{}] of a ({}x{}) matrix.",
                                   i1, i2, j1, j2, rows_, cols_));
            return None<self>;
        }
        self ans(i2 - i1 + 1, j2 - j1 + 1);
        for (c_size i = 0; i < ans.rows_; ++i) {
            for (c_size j = 0; j < ans.cols_; ++j) {
                ans[i][j] = data_[i1 + i][j1 + j];
            }
        }
        return ans;
    }

    /**
     * @brief 填充矩阵
     */
    void fill(value_t value) {
        for (c_size i = 0; i < rows_; ++i) {
            for (c_size j = 0; j < cols_; ++j) {
                data_[i][j] = value;
            }
        }
    }

    bool shapeEquals(const self& other) const {
        return this->rows_ == other.rows_ && this->cols_ == other.cols_;
    }

    friend self operator+(const self& a, const self& b) {
        if (!a.shapeEquals(b)) {
            ValueError(std::format("Cannot add a ({}x{}) matrix and a ({}x{}) matrix.", a.rows_, a.cols_, b.rows_, b.cols_));
            return None<self>;
        }
        self ans(a.rows_, a.cols_);
        for (c_size i = 0; i < a.rows_; ++i) {
            for (c_size j = 0; j < a.cols_; ++j) {
                ans[i][j] = a[i][j] + b[i][j];
            }
        }
        return ans;
    }

    self& operator+=(const self& other) {
        *this = *this + other;
        return *this;
    }

    friend self operator-(const self& a, const self& b) {
        if (!a.shapeEquals(b)) {
            ValueError(std::format("Cannot subtract a ({}x{}) matrix and a ({}x{}) matrix.", a.rows_, a.cols_, b.rows_, b.cols_));
            return None<self>;
        }
        self ans(a.rows_, a.cols_);
        for (c_size i = 0; i < a.rows_; ++i) {
            for (c_size j = 0; j < a.cols_; ++j) {
                ans[i][j] = a[i][j] - b[i][j];
            }
        }
        return ans;
    }

    self& operator-=(const self& other) {
        *this = *this - other;
        return *this;
    }

    friend self operator*(const self& a, const self& b) {
        if (a.cols_ != b.rows_) {
            ValueError("To be multiplied, the cols of matrix A must equals to the rows of matrix B.");
            return None<self>;
        }
        self ans(a.rows_, b.cols_);
        for (c_size i = 0; i < a.rows_; ++i) {
            for (c_size j = 0; j < b.cols_; ++j) {
                for (c_size k = 0; k < a.cols_; ++k) {
                    ans[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        return ans;
    }

    self& operator*=(const self& other) {
        *this = *this * other;
        return *this;
    }

    /**
     * @brief 点乘
     */
    self dot(const self& other) const {
        if (!this->shapeEquals(other)) {
            ValueError(std::format("Cannot dot a ({}x{}) matrix and a ({}x{}) matrix.",
                                   this->rows_, this->cols_, other.rows_, other.cols_));
            return None<self>;
        }
        self ans(this->rows_, this->cols_);
        for (c_size i = 0; i < ans.rows_; ++i) {
            for (c_size j = 0; j < ans.cols_; ++j) {
                ans[i][j] = this->data_[i][j] * other[i][j];
            }
        }
        return ans;
    }

    self dot(value_t value) const {
        self ans(rows_, cols_);
        for (c_size i = 0; i < ans.rows_; ++i) {
            for (c_size j = 0; j < ans.cols_; ++j) {
                ans[i][j] = this->data_[i][j] * value;
            }
        }
        return ans;
    }

    /**
     * @brief 求转置
     */
    self T() const {
        self ans(cols_, rows_);
        for (c_size i = 0; i < rows_; ++i) {
            for (c_size j = 0; j < cols_; ++j) {
                ans[j][i] = data_[i][j];
            }
        }
        return ans;
    }

    /**
     * @brief 交换两行
     * @param i 行号，从0开始
     * @param j 行号，从0开始
     * @return 是否交换，true=是 false=否
     */
    bool swapRow(c_size i, c_size j) {
        if (i == j) return false;
        if (i < 0 || i >= rows_ || j < 0 || j >= rows_) {
            ValueError(std::format("Invalid line number [{}] or [{}]", i, j));
            return None<bool>;
        }
        for (c_size k = 0; k < cols_; ++k) {
            std::swap(data_[i][k], data_[j][k]);
        }
        return true;
    }

    /**
     * @brief 交换两列
     * @param i 列号，从0开始
     * @param j 列号，从0开始
     * @return 是否交换，true=是 false=否
     */
    bool swapCol(c_size i, c_size j) {
        if (i == j) return false;
        if (i < 0 || i >= cols_ || j < 0 || j >= cols_) {
            ValueError(std::format("Invalid line number [{}] or [{}]", i, j));
            return None<bool>;
        }
        for (c_size k = 0; k < rows_; ++k) {
            std::swap(data_[k][i], data_[k][j]);
        }
        return true;
    }

    /**
     * @brief 求逆
     */
    self inv() const {
        if (!isSquare()) {
            ValueError("Only square matrices are LU decomposition.");
            return None<self>;
        }

        value_t p, d;
        self ans = this->clone();
        util::Array<c_size> is(rows_); // 记录行交换
        util::Array<c_size> js(cols_); // 记录列交换
        for (c_size k = 0; k < rows_; ++k) {
            p = 0.0;
            for (c_size i = k; i < rows_; ++i) {
                for (c_size j = k; j < cols_; ++j) {
                    d = std::fabs(ans[i][j]);
                    if (d > p) {
                        p = d;
                        is[k] = i;
                        js[k] = j;
                    }
                }
            }
            checkPivot(p);
            ans.swapRow(k, is[k]);
            ans.swapCol(k, js[k]);
            ans[k][k] = reciprocal(ans[k][k]); // 归一化主元
            for (c_size j = 0; j < cols_; ++j) {
                if (j == k) continue;
                ans[k][j] *= ans[k][k];
            }
            for (c_size i = 0; i < rows_; ++i) {
                if (i == k) continue;
                for (c_size j = 0; j < cols_; ++j) {
                    if (j == k) continue;
                    ans[i][j] -= ans[i][k] * ans[k][j];
                }
            }
            for (c_size i = 0; i < rows_; ++i) {
                if (i == k) continue;
                ans[i][k] *= (-ans[k][k]);
            }
        }

        // 逆交换，还原行和列
        for (c_size k = rows_ - 1; k >= 0; --k) {
            ans.swapRow(k, js[k]);
            ans.swapCol(k, is[k]);
        }
        ans.correct();
        return ans;
    }

    /**
     * @brief 求行列式
     */
    value_t det() const {
        if (!isSquare()) {
            ValueError("Only square matrices are LU decomposition.");
            return None<value_t>;
        }

        value_t d, p;
        c_size is, js;
        self m = this->clone();
        value_t f = 1.0, ans = 1.0; // 符号因子，行列式值
        for (c_size k = 0; k < rows_ - 1; ++k) {
            p = 0.0;
            for (c_size i = k; i < rows_; ++i) {
                for (c_size j = k; j < cols_; ++j) {
                    d = std::fabs(m[i][j]);
                    if (d > p) {
                        p = d;
                        is = i;
                        js = j;
                    }
                }
            }
            p = correctFloat(p);
            if (m.swapRow(k, is)) f = -f;
            if (m.swapCol(k, js)) f = -f;
            ans *= m[k][k];
            for (c_size i = k + 1; i < rows_; ++i) {
                d = m[i][k] / m[k][k];
                for (c_size j = k + 1; j < cols_; ++j) {
                    m[i][j] -= d * m[k][j];
                }
            }
        }
        ans *= f * m[rows_ - 1][cols_ - 1];
        return ans;
    }

    /**
     * @brief 求秩
     */
    i32 rank() const {
        i32 ans = 0;
        value_t d, p;
        self m = this->clone();
        c_size n = std::min(rows_, cols_), is, js;
        for (int k = 0; k < n; ++k) {
            p = 0.0;
            for (int i = 1; i < rows_; ++i) {
                for (int j = 1; j < cols_; ++j) {
                    d = std::fabs(m[i][j]);
                    if (d > p) {
                        p = d;
                        is = i;
                        js = j;
                    }
                }
            }
            if (isZero(p)) return ans;
            ++ans;
            m.swapRow(k, is);
            m.swapCol(k, js);
            for (c_size i = k + 1; i < rows_; ++i) {
                d = m[i][k] / m[k][k];
                for (c_size j = k + 1; j < cols_; ++j) {
                    m[i][j] -= d * m[k][j];
                }
            }
        }
        return ans;
    }

    /**
     * @brief LU分解。使用高斯-约当消元
     * @return pair的first为L，second为U
     */
    Pair<self, self> LU() const {
        if (!isSquare()) {
            ValueError("Only square matrices are LU decomposition.");
            return None<Pair<self, self>>;
        }

        self q = this->clone();
        for (c_size k = 0; k < q.rows_ - 1; ++k) {
            checkPivot(q[k][k]);
            for (c_size i = k + 1; i < rows_; ++i) {
                q[i][k] /= q[k][k];
                for (c_size j = k + 1; j < cols_; ++j) {
                    q[i][j] -= q[i][k] * q[k][j];
                }
            }
        }

        self l(rows_, cols_), u(rows_, cols_);
        for (c_size i = 0; i < rows_; ++i) {
            for (c_size j = 0; j < i; ++j) {
                l[i][j] = q[i][j];
                u[i][j] = 0;
            }
            l[i][i] = 1;
            u[i][i] = q[i][i];
            for (c_size j = i + 1; j < cols_; ++j) {
                l[i][j] = 0;
                u[i][j] = q[i][j];
            }
        }
        return Pair{l, u};
    }

    /**
     * @brief 判断是否为方阵
     * @return true=是 false=否
     */
    bool isSquare() const {
        return rows_ == cols_;
    }

    self clone() const {
        self ans(this->rows_, this->cols_);
        for (c_size i = 0; i < ans.rows_; ++i) {
            for (c_size j = 0; j < ans.cols_; ++j) {
                ans[i][j] = data_[i][j];
            }
        }
        return ans;
    }

    CString __str__() const {
        return data_.__str__();
    }

    cmp_t __cmp__(const self& other) const {
        if (this->rows_ != other.rows_ || this->cols_ != other.cols_) {
            ValueError("Only matrices of the same dimension are comparable");
            return None<cmp_t>;
        }
        for (c_size i = 0; i < rows_; ++i) {
            for (c_size j = 0; j < cols_; ++j) {
                if (compare(this->data_[i][j], other[i][j]) > 0) {
                    return 1;
                } else if (compare(this->data_[i][j], other[i][j]) < 0) {
                    return -1;
                }
            }
        }
        return 0;
    }

private:
    /**
     * @brief 校验高斯-约当消元法的主元，不能为0
     */
    static void checkPivot(value_t pivot) {
        if (isZero(pivot)) {
            ValueError("Pivot entries cannot be 0.");
        }
    }

    void correct() {
        for (c_size i = 0; i < rows_; ++i) {
            for (c_size j = 0; j < cols_; ++j) {
                data_[i][j] = correctFloat(data_[i][j]);
            }
        }
    }

private:
    c_size rows_; // 行数
    c_size cols_; // 列数
    util::DynArray<util::DynArray<value_t>> data_;
};

} // namespace my::math

#endif // MATRIX_HPP