/**
 * @brief 矩阵
 * @author Ricky
 * @date 2025/1/14
 * @version 1.0
 */
#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "math_utils.hpp"
#include "Vec.hpp"

namespace my::math {

template <typename E>
class Matrix;

/**
 * @brief 矩阵视图
 */
template <typename E>
class MatrixView : public Object<MatrixView<E>> {
public:
    using value_t = E;
    using Self = MatrixView<value_t>;

    MatrixView(const Matrix<value_t>& ref, isize row_begin, isize col_begin, isize rows, isize cols) :
            ref_(ref), row_begin_(row_begin), col_begin_(col_begin), rows_(rows), cols_(cols) {}

    isize rows() const {
        return rows_;
    }

    isize cols() const {
        return cols_;
    }

    typename Matrix<value_t>::ConstRowProxy operator[](isize i) const {
        return ref_[row_begin_ + i];
    }

    value_t at(isize i, isize j) const {
        return ref_.at(row_begin_ + i, col_begin_ + j);
    }

    Matrix<value_t> to_matrix() const {
        Matrix<value_t> result(rows_, cols_);
        for (isize i = 0; i < rows_; ++i) {
            for (isize j = 0; j < cols_; ++j) {
                result(i, j) = at(i, j);
            }
        }
        return result;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (isize i = 0; i < rows_; ++i) {
            stream << '[';
            for (isize j = 0; j < cols_; ++j) {
                stream << at(i, j);
                if (j != cols_ - 1) {
                    stream << ',';
                }
            }
            stream << ']';
            if (i != rows_ - 1) {
                stream << ',';
            }
        }
        stream << ']';
        return CString{stream.str()};
    }

private:
    const Matrix<value_t>& ref_;
    isize row_begin_, col_begin_;
    isize rows_, cols_;
};

/**
 * @brief 矩阵
 */
template <typename E>
class Matrix : public Object<Matrix<E>> {
public:
    using value_t = E;
    using Self = Matrix<value_t>;

    class RowProxy;
    class ConstRowProxy;

    Matrix(isize rows = 1, isize cols = 1, value_t value = 0.0) :
            rows_(rows), cols_(cols), data_(rows_ * cols_, value) {}

    Matrix(isize n, value_t value = 0.0) :
            rows_(n), cols_(n), data_(rows_ * cols_, value) {}

    Matrix(std::initializer_list<std::initializer_list<value_t>>&& init_list) :
            rows_(init_list.size()), cols_(rows_ > 0 ? init_list.begin()->size() : 0), data_(rows_ * cols_) {
        size_t index = 0;
        for (auto&& row : init_list) {
            if (isize(row.size()) != cols_) {
                ValueError("Inconsistent row sizes in initializer list");
            }
            for (auto&& item : row) {
                data_.at(index++) = std::move(item);
            }
        }
    }

    /**
     * @brief 对角阵
     */
    Matrix(std::initializer_list<value_t>&& init_list) :
            Matrix(init_list.size(), init_list.size()) {
        isize pos = 0;
        for (auto&& item : init_list) {
            at_impl(pos, pos) = item;
            ++pos;
        }
    }

    isize rows() const {
        return rows_;
    }

    isize cols() const {
        return cols_;
    }

    /**
     * @brief 判断是否为方阵
     * @return true=是 false=否
     */
    bool is_sqr() const {
        return rows_ == cols_;
    }

    /**
     * @brief 判断是否形状相同
     * @return true=是 false=否
     */
    bool shape_equals(const Self& other) const {
        return this->rows_ == other.rows_ && this->cols_ == other.cols_;
    }

    value_t& at(isize i, isize j) {
        if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
            ValueError(std::format("Index[{}, {}] out of range", i, j));
            std::unreachable();
        }
        return at_impl(i, j);
    }

    const value_t& at(isize i, isize j) const {
        if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
            ValueError(std::format("Index[{}, {}] out of range", i, j));
            std::unreachable();
        }
        return at_impl(i, j);
    }

    value_t& operator()(isize i, isize j) {
        return at_impl(i, j);
    }

    const value_t& operator()(isize i, isize j) const {
        return at_impl(i, j);
    }

    RowProxy operator[](isize i) {
        if (i < 0 || i >= rows_) {
            ValueError(std::format("Row index[{}] out of range", i));
            std::unreachable();
        }
        return RowProxy{data_, i * cols_, cols_};
    }

    ConstRowProxy operator[](isize i) const {
        if (i < 0 || i >= rows_) {
            ValueError(std::format("Row index[{}] out of range", i));
            std::unreachable();
        }
        return ConstRowProxy{data_, i * cols_, cols_};
    }

    /**
     * @brief 获取当前矩阵的子矩阵，行列均为闭区间
     */
    MatrixView<value_t> sub_mat(isize i1, isize j1, isize i2, isize j2) const {
        if (i1 > i2 || j1 > j2 || i1 < 0 || j1 < 0 || i2 >= rows_ || j2 >= cols_) {
            ValueError(std::format("Cannot get submatrix [{}..{}] x [{}..{}] of a ({}x{}) matrix.",
                                   i1, i2, j1, j2, rows_, cols_));
            std::unreachable();
        }
        return MatrixView<value_t>{*this, i1, j1, i2 - i1 + 1, j2 - j1 + 1};
    }

    /**
     * @brief 填充矩阵
     */
    void fill(value_t&& value) {
        for (auto&& elem : data_) {
            elem = std::move(value);
        }
    }

    Self clone() const {
        Self ans(this->rows_, this->cols_);
        isize size = this->data_.size();
        for (isize i = 0; i < size; ++i) {
            ans.data_[i] = this->data_[i];
        }
        return ans;
    }

    friend Self operator+(const Self& a, const Self& b) {
        if (!a.shape_equals(b)) {
            ValueError(std::format("Cannot add a ({}x{}) matrix and a ({}x{}) matrix.", a.rows_, a.cols_, b.rows_, b.cols_));
            std::unreachable();
        }
        Self ans(a.rows_, a.cols_);
        isize size = ans.data_.size();
        for (isize i = 0; i < size; ++i) {
            ans.data_[i] = a.data_[i] + b.data_[i];
        }
        return ans;
    }

    Self& operator+=(const Self& other) {
        if (!this->shape_equals(other)) {
            ValueError(std::format("Cannot add a ({}x{}) matrix and a ({}x{}) matrix.",
                                   this->rows_, this->cols_, other.rows_, other.cols_));
            std::unreachable();
        }
        isize size = this->data_.size();
        for (isize i = 0; i < size; ++i) {
            this->data_[i] += other.data_[i];
        }
        return *this;
    }

    friend Self operator-(const Self& a, const Self& b) {
        if (!a.shape_equals(b)) {
            ValueError(std::format("Cannot substract a ({}x{}) matrix and a ({}x{}) matrix.", a.rows_, a.cols_, b.rows_, b.cols_));
            std::unreachable();
        }
        Self ans(a.rows_, a.cols_);
        isize size = ans.data_.size();
        for (isize i = 0; i < size; ++i) {
            ans.data_[i] = a.data_[i] - b.data_[i];
        }
        return ans;
    }

    Self& operator-=(const Self& other) {
        if (!this->shape_equals(other)) {
            ValueError(std::format("Cannot substract a ({}x{}) matrix and a ({}x{}) matrix.",
                                   this->rows_, this->cols_, other.rows_, other.cols_));
            std::unreachable();
        }
        isize size = this->data_.size();
        for (isize i = 0; i < size; ++i) {
            this->data_[i] -= other.data_[i];
        }
        return *this;
    }

    friend Self operator*(const Self& a, const Self& b) {
        if (a.cols_ != b.rows_) {
            ValueError("To be multiplied, the cols of matrix A must equals to the rows of matrix B.");
            std::unreachable();
        }
        Self result(a.rows_, b.cols_);
        for (isize i = 0; i < a.rows_; ++i) {
            for (isize k = 0; k < a.cols_; ++k) {
                auto a_ik = a(i, k);
                for (isize j = 0; j < b.cols_; ++j) {
                    result(i, j) += a_ik * b(k, j);
                }
            }
        }
        return result;
    }

    Self& operator*=(const Self& other) {
        *this = *this * other;
        return *this;
    }

    /**
     * @brief 点乘
     */
    Self dot(const Self& other) const {
        if (!this->shape_equals(other)) {
            ValueError(std::format("Cannot dot a ({}x{}) matrix and a ({}x{}) matrix.",
                                   this->rows_, this->cols_, other.rows_, other.cols_));
            std::unreachable();
        }
        Self ans(this->rows_, this->cols_);
        isize size = ans.data_.size();
        for (isize i = 0; i < size; ++i) {
            ans.data_[i] = this->data_[i] * other.data_[i];
        }
        return ans;
    }

    Self dot(value_t value) const {
        Self ans(rows_, cols_);
        isize size = ans.data_.size();
        for (isize i = 0; i < size; ++i) {
            ans.data_[i] = this->data_[i] * value;
        }
        return ans;
    }

    /**
     * @brief 求转置
     */
    Self t() const {
        Self ans(cols_, rows_);
        for (isize i = 0; i < rows_; ++i) {
            for (isize j = 0; j < cols_; ++j) {
                ans(j, i) = this->at_impl(i, j);
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
    bool swap_row(isize i, isize j) {
        if (i == j) return false;
        if (i < 0 || i >= rows_ || j < 0 || j >= rows_) {
            ValueError(std::format("Invalid line number [{}] or [{}]", i, j));
            std::unreachable();
        }
        for (isize k = 0; k < cols_; ++k) {
            std::swap(at_impl(i, k), at_impl(j, k));
        }
        return true;
    }

    /**
     * @brief 交换两列
     * @param i 列号，从0开始
     * @param j 列号，从0开始
     * @return 是否交换，true=是 false=否
     */
    bool swap_col(isize i, isize j) {
        if (i == j) return false;
        if (i < 0 || i >= cols_ || j < 0 || j >= cols_) {
            ValueError(std::format("Invalid line number [{}] or [{}]", i, j));
            std::unreachable();
        }
        for (isize k = 0; k < rows_; ++k) {
            std::swap(at_impl(k, i), at_impl(k, j));
        }
        return true;
    }

    /**
     * @brief 求逆
     */
    Self inv() const {
        if (!is_sqr()) {
            ValueError("Only square matrices are LU decomposition.");
            std::unreachable();
        }

        value_t p, d;
        Self ans = this->clone();
        util::Array<isize> is(rows_); // 记录行交换
        util::Array<isize> js(cols_); // 记录列交换
        for (isize k = 0; k < rows_; ++k) {
            p = 0.0;
            for (isize i = k; i < rows_; ++i) {
                for (isize j = k; j < cols_; ++j) {
                    d = std::fabs(ans(i, j));
                    if (d > p) {
                        p = d;
                        is[k] = i;
                        js[k] = j;
                    }
                }
            }
            check_pivot(p);
            ans.swap_row(k, is[k]);
            ans.swap_col(k, js[k]);
            ans(k, k) = reciprocal(ans(k, k)); // 归一化主元
            for (isize j = 0; j < cols_; ++j) {
                if (j == k) continue;
                ans(k, j) *= ans(k, k);
            }
            for (isize i = 0; i < rows_; ++i) {
                if (i == k) continue;
                for (isize j = 0; j < cols_; ++j) {
                    if (j == k) continue;
                    ans(i, j) -= ans(i, k) * ans(k, j);
                }
            }
            for (isize i = 0; i < rows_; ++i) {
                if (i == k) continue;
                ans(i, k) *= (-ans(k, k));
            }
        }

        // 逆交换，还原行和列
        for (isize k = rows_ - 1; k >= 0; --k) {
            ans.swap_row(k, js[k]);
            ans.swap_col(k, is[k]);
        }
        ans.correct();
        return ans;
    }

    /**
     * @brief 求行列式
     */
    value_t det() const {
        if (!is_sqr()) {
            ValueError("Only square matrices are LU decomposition.");
            std::unreachable();
        }

        value_t d, p;
        isize is, js;
        Self m = this->clone();
        value_t f = 1.0, ans = 1.0; // 符号因子，行列式值
        for (isize k = 0; k < rows_ - 1; ++k) {
            p = 0.0;
            for (isize i = k; i < rows_; ++i) {
                for (isize j = k; j < cols_; ++j) {
                    d = std::fabs(m(i, j));
                    if (d > p) {
                        p = d;
                        is = i;
                        js = j;
                    }
                }
            }
            p = correct_float(p);
            if (m.swap_row(k, is)) f = -f;
            if (m.swap_col(k, js)) f = -f;
            ans *= m(k, k);
            for (isize i = k + 1; i < rows_; ++i) {
                d = m(i, k) / m(k, k);
                for (isize j = k + 1; j < cols_; ++j) {
                    m(i, j) -= d * m(k, j);
                }
            }
        }
        ans *= f * m(rows_ - 1, cols_ - 1);
        return ans;
    }

    /**
     * @brief 求秩
     */
    i32 rank() const {
        i32 ans = 0;
        value_t d, p;
        Self m = this->clone();
        isize n = std::min(rows_, cols_), is, js;
        for (int k = 0; k < n; ++k) {
            p = 0.0;
            for (int i = 1; i < rows_; ++i) {
                for (int j = 1; j < cols_; ++j) {
                    d = std::fabs(m(i, j));
                    if (d > p) {
                        p = d;
                        is = i;
                        js = j;
                    }
                }
            }
            if (is_zero(p)) return ans;
            ++ans;
            m.swap_row(k, is);
            m.swap_col(k, js);
            for (isize i = k + 1; i < rows_; ++i) {
                d = m(i, k) / m(k, k);
                for (isize j = k + 1; j < cols_; ++j) {
                    m(i, j) -= d * m(k, j);
                }
            }
        }
        return ans;
    }

    /**
     * @brief LU分解。使用高斯-约当消元
     * @return pair的first为L，second为U
     */
    Pair<Self, Self> lu() const {
        if (!is_sqr()) {
            ValueError("Only square matrices are LU decomposition.");
            std::unreachable();
        }

        Self q = this->clone();
        for (isize k = 0; k < q.rows_ - 1; ++k) {
            check_pivot(q(k, k));
            for (isize i = k + 1; i < rows_; ++i) {
                q(i, k) /= q(k, k);
                for (isize j = k + 1; j < cols_; ++j) {
                    q(i, j) -= q(i, k) * q(k, j);
                }
            }
        }

        Self l(rows_, cols_), u(rows_, cols_);
        for (isize i = 0; i < rows_; ++i) {
            for (isize j = 0; j < i; ++j) {
                l(i, j) = q(i, j);
                u(i, j) = 0;
            }
            l(i, i) = 1;
            u(i, i) = q(i, i);
            for (isize j = i + 1; j < cols_; ++j) {
                l(i, j) = 0;
                u(i, j) = q(i, j);
            }
        }
        return Pair{l, u};
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (isize i = 0; i < rows_; ++i) {
            stream << '[';
            for (isize j = 0; j < cols_; ++j) {
                stream << at_impl(i, j);
                if (j != cols_ - 1) {
                    stream << ',';
                }
            }
            stream << ']';
            if (i != rows_ - 1) {
                stream << ',';
            }
        }
        stream << ']';
        return CString{stream.str()};
    }

    [[nodiscard]] cmp_t __cmp__(const Self& other) const {
        if (!this->shape_equals(other)) {
            ValueError("Only matrices of the same dimension are comparable");
            std::unreachable();
        }
        isize size = this->data_.size();
        for (isize i = 0; i < size; ++i) {
            if (fcmp(this->data_[i], other.data_[i]) > 0) {
                return 1;
            } else if (fcmp(this->data_[i], other.data_[i]) < 0) {
                return -1;
            }
        }
        return 0;
    }

private:
    value_t& at_impl(isize i, isize j) {
        return data_.at(i * cols_ + j);
    }

    const value_t& at_impl(isize i, isize j) const {
        return data_.at(i * cols_ + j);
    }

    /**
     * @brief 校验高斯-约当消元法的主元，不能为0
     */
    static void check_pivot(value_t pivot) {
        if (is_zero(pivot)) {
            ValueError("Pivot entries cannot be 0.");
        }
    }

    void correct() {
        isize size = data_.size();
        for (isize i = 0; i < size; ++i) {
            data_[i] = correct_float(data_[i]);
        }
    }

public:
    class RowProxy {
    public:
        RowProxy(util::Vec<value_t>& data, isize start_col, isize cols) :
                data_(data), start_col_(start_col), cols_(cols) {}

        value_t& operator[](isize j) {
            if (j < 0 || j >= cols_) {
                ValueError("Column index out of range");
                std::unreachable();
            }
            return data_[start_col_ + j];
        }

    private:
        util::Vec<value_t>& data_;
        isize start_col_;
        isize cols_;
    };

    class ConstRowProxy {
    public:
        ConstRowProxy(const util::Vec<value_t>& data, isize start_col, isize cols) :
                data_(data), start_col_(start_col), cols_(cols) {}

        const value_t& operator[](isize j) const {
            if (j < 0 || j >= cols_) {
                ValueError("Column index out of range");
                std::unreachable();
            }
            return data_[start_col_ + j];
        }

    private:
        const util::Vec<value_t>& data_;
        isize start_col_;
        isize cols_;
    };

private:
    isize rows_;              // 行数
    isize cols_;              // 列数
    util::Vec<value_t> data_; // 一维存储，提高空间局部性
};

} // namespace my::math

#endif // MATRIX_HPP