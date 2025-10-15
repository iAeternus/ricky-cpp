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

    MatrixView(const Matrix<value_t>& ref, usize row_begin, usize col_begin, usize rows, usize cols) :
            ref_(ref), row_begin_(row_begin), col_begin_(col_begin), rows_(rows), cols_(cols) {}

    usize rows() const {
        return rows_;
    }

    usize cols() const {
        return cols_;
    }

    typename Matrix<value_t>::ConstRowView operator[](usize i) const {
        return ref_[row_begin_ + i];
    }

    /**
     * @brief 相对下标访问
     */
    value_t at(usize i, usize j) const {
        return ref_.at(row_begin_ + i, col_begin_ + j);
    }

    /**
     * @brief 绝对下标访问
     */
    value_t at_abs(usize i, usize j) const {
        return ref_.at(i, j);
    }

    /**
     * @brief 视图的子视图，行列从0开始均为闭区间
     */
    Self sub_view(usize i1, usize j1, usize i2, usize j2) const {
        return Self{ref_, row_begin_ + i1, col_begin_ + j1, i2 - i1 + 1, j2 - j1 + 1};
    }

    Matrix<value_t> to_matrix() const {
        Matrix<value_t> result(rows_, cols_);
        for (usize i = 0; i < rows_; ++i) {
            for (usize j = 0; j < cols_; ++j) {
                result(i, j) = at(i, j);
            }
        }
        return result;
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (usize i = 0; i < rows_; ++i) {
            stream << '[';
            for (usize j = 0; j < cols_; ++j) {
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
    usize row_begin_, col_begin_;
    usize rows_, cols_;
};

/**
 * @brief 矩阵
 */
template <typename E>
class Matrix : public Object<Matrix<E>> {
public:
    using value_t = E;
    using Self = Matrix<value_t>;

public:
    /**
     * @brief 行视图
     */
    class RowView {
    public:
        RowView(util::Vec<value_t>& data, usize start_col, usize cols) :
                data_(data), start_col_(start_col), cols_(cols) {}

        value_t& operator[](usize j) {
            if (j >= cols_) {
                throw index_out_of_bounds_exception("Column index {} out of bounds [0..{}]", j, cols_);
            }
            return data_[start_col_ + j];
        }

    private:
        util::Vec<value_t>& data_;
        usize start_col_;
        usize cols_;
    };

    /**
     * @brief 行视图，常量版本
     */
    class ConstRowView {
    public:
        ConstRowView(const util::Vec<value_t>& data, usize start_col, usize cols) :
                data_(data), start_col_(start_col), cols_(cols) {}

        const value_t& operator[](usize j) const {
            if (j >= cols_) {
                throw index_out_of_bounds_exception("Column index {} out of bounds [0..{}]", j, cols_);
            }
            return data_[start_col_ + j];
        }

    private:
        const util::Vec<value_t>& data_;
        usize start_col_;
        usize cols_;
    };

    Matrix(usize rows = 1, usize cols = 1, value_t value = 0.0) :
            rows_(rows), cols_(cols), data_(rows_ * cols_, value) {}

    Matrix(std::initializer_list<std::initializer_list<value_t>>&& init_list) :
            rows_(init_list.size()), cols_(rows_ > 0 ? init_list.begin()->size() : 0), data_(rows_ * cols_) {
        size_t index = 0;
        for (auto&& row : init_list) {
            if (usize(row.size()) != cols_) {
                throw argument_exception("Inconsistent row sizes in initializer list");
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
        usize pos = 0;
        for (auto&& item : init_list) {
            at_impl(pos, pos) = item;
            ++pos;
        }
    }

    Matrix(const Self& other) = default;
    Matrix(Self&& other) noexcept = default;

    Self& operator=(const Self& other) = default;
    Self& operator=(Self&& other) noexcept = default;

    usize rows() const {
        return rows_;
    }

    usize cols() const {
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

    value_t& at(usize i, usize j) {
        if (i >= rows_ || j >= cols_) {
            throw index_out_of_bounds_exception("Index [{}, {}] out of bounds [0..{}, 0..{}]", i, j, rows_, cols_);
        }
        return at_impl(i, j);
    }

    const value_t& at(usize i, usize j) const {
        if (i >= rows_ || j >= cols_) {
            throw index_out_of_bounds_exception("Index [{}, {}] out of bounds [0..{}, 0..{}]", i, j, rows_, cols_);
        }
        return at_impl(i, j);
    }

    value_t& operator()(usize i, usize j) {
        return at_impl(i, j);
    }

    const value_t& operator()(usize i, usize j) const {
        return at_impl(i, j);
    }

    RowView operator[](usize i) {
        if (i >= rows_) {
            throw index_out_of_bounds_exception("row index {} out of range [0..{}]", i, rows_);
        }
        return RowView{data_, i * cols_, cols_};
    }

    ConstRowView operator[](usize i) const {
        if (i >= rows_) {
            throw index_out_of_bounds_exception("row index {} out of range [0..{}]", i, rows_);
        }
        return ConstRowView{data_, i * cols_, cols_};
    }

    /**
     * @brief 获取当前矩阵的子矩阵，行列从0开始均为闭区间
     */
    MatrixView<value_t> sub_mat(usize i1, usize j1, usize i2, usize j2) const {
        if (i1 > i2 || j1 > j2 || i2 >= rows_ || j2 >= cols_) {
            throw arithmetic_exception("cannot get submatrix [{}..{}] x [{}..{}] of a ({}x{}) matrix.",
                                       i1, i2, j1, j2, rows_, cols_);
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
        usize size = this->data_.size();
        for (usize i = 0; i < size; ++i) {
            ans.data_[i] = this->data_[i];
        }
        return ans;
    }

    friend Self operator+(const Self& a, const Self& b) {
        if (!a.shape_equals(b)) {
            throw arithmetic_exception("cannot add a ({}x{}) matrix and a ({}x{}) matrix.",
                                       a.rows_, a.cols_, b.rows_, b.cols_);
        }
        Self ans(a.rows_, a.cols_);
        usize size = ans.data_.size();
        for (usize i = 0; i < size; ++i) {
            ans.data_[i] = a.data_[i] + b.data_[i];
        }
        return ans;
    }

    Self& operator+=(const Self& other) {
        if (!this->shape_equals(other)) {
            throw arithmetic_exception("cannot add a ({}x{}) matrix and a ({}x{}) matrix.",
                                       this->rows_, this->cols_, other.rows_, other.cols_);
        }
        usize size = this->data_.size();
        for (usize i = 0; i < size; ++i) {
            this->data_[i] += other.data_[i];
        }
        return *this;
    }

    friend Self operator-(const Self& a, const Self& b) {
        if (!a.shape_equals(b)) {
            throw arithmetic_exception("cannot substract a ({}x{}) matrix and a ({}x{}) matrix.",
                                       a.rows_, a.cols_, b.rows_, b.cols_);
        }
        Self ans(a.rows_, a.cols_);
        usize size = ans.data_.size();
        for (usize i = 0; i < size; ++i) {
            ans.data_[i] = a.data_[i] - b.data_[i];
        }
        return ans;
    }

    Self& operator-=(const Self& other) {
        if (!this->shape_equals(other)) {
            throw arithmetic_exception("cannot substract a ({}x{}) matrix and a ({}x{}) matrix.",
                                       this->rows_, this->cols_, other.rows_, other.cols_);
        }
        usize size = this->data_.size();
        for (usize i = 0; i < size; ++i) {
            this->data_[i] -= other.data_[i];
        }
        return *this;
    }

    friend Self operator*(const Self& a, const Self& b) {
        if (a.cols_ != b.rows_) {
            throw arithmetic_exception("cannot multiply a ({}x{}) matrix and a ({}x{}) matrix.",
                                       a.rows_, a.cols_, b.rows_, b.cols_);
        }
        Self result(a.rows_, b.cols_);
        for (usize i = 0; i < a.rows_; ++i) {
            for (usize k = 0; k < a.cols_; ++k) {
                auto a_ik = a(i, k);
                for (usize j = 0; j < b.cols_; ++j) {
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
            throw arithmetic_exception("cannot dot a ({}x{}) matrix and a ({}x{}) matrix.",
                                       this->rows_, this->cols_, other.rows_, other.cols_);
        }
        Self ans(this->rows_, this->cols_);
        usize size = ans.data_.size();
        for (usize i = 0; i < size; ++i) {
            ans.data_[i] = this->data_[i] * other.data_[i];
        }
        return ans;
    }

    Self dot(value_t value) const {
        Self ans(rows_, cols_);
        usize size = ans.data_.size();
        for (usize i = 0; i < size; ++i) {
            ans.data_[i] = this->data_[i] * value;
        }
        return ans;
    }

    /**
     * @brief 求转置
     */
    Self t() const {
        Self ans(cols_, rows_);
        for (usize i = 0; i < rows_; ++i) {
            for (usize j = 0; j < cols_; ++j) {
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
    bool swap_row(usize i, usize j) {
        if (i == j) return false;
        if (i >= rows_ || j >= rows_) {
            throw index_out_of_bounds_exception("row index {} or {} out of range [0..{}]", i, j, rows_);
        }
        for (usize k = 0; k < cols_; ++k) {
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
    bool swap_col(usize i, usize j) {
        if (i == j) return false;
        if (i >= cols_ || j >= cols_) {
            throw index_out_of_bounds_exception("column index {} or {} out of range [0..{}]", i, j, cols_);
        }
        for (usize k = 0; k < rows_; ++k) {
            std::swap(at_impl(k, i), at_impl(k, j));
        }
        return true;
    }

    /**
     * @brief 求逆
     */
    Self inv() const {
        if (!is_sqr()) {
            throw arithmetic_exception("only square matrices have inverse matrices.");
        }

        value_t p, d;
        Self ans = this->clone();
        util::Array<usize> is(rows_); // 记录行交换
        util::Array<usize> js(cols_); // 记录列交换
        for (usize k = 0; k < rows_; ++k) {
            p = 0.0;
            for (usize i = k; i < rows_; ++i) {
                for (usize j = k; j < cols_; ++j) {
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
            for (usize j = 0; j < cols_; ++j) {
                if (j == k) continue;
                ans(k, j) *= ans(k, k);
            }
            for (usize i = 0; i < rows_; ++i) {
                if (i == k) continue;
                for (usize j = 0; j < cols_; ++j) {
                    if (j == k) continue;
                    ans(i, j) -= ans(i, k) * ans(k, j);
                }
            }
            for (usize i = 0; i < rows_; ++i) {
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
            throw arithmetic_exception("only square matrices can have their determinants calculated.");
        }

        value_t d, p;
        usize is, js;
        Self m = this->clone();
        value_t f = 1.0, ans = 1.0; // 符号因子，行列式值
        for (usize k = 0; k < rows_ - 1; ++k) {
            p = 0.0;
            for (usize i = k; i < rows_; ++i) {
                for (usize j = k; j < cols_; ++j) {
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
            for (usize i = k + 1; i < rows_; ++i) {
                d = m(i, k) / m(k, k);
                for (usize j = k + 1; j < cols_; ++j) {
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
        usize n = std::min(rows_, cols_), is, js;
        for (i32 k = 0; k < n; ++k) {
            p = 0.0;
            for (i32 i = 1; i < rows_; ++i) {
                for (i32 j = 1; j < cols_; ++j) {
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
            for (usize i = k + 1; i < rows_; ++i) {
                d = m(i, k) / m(k, k);
                for (usize j = k + 1; j < cols_; ++j) {
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
            throw arithmetic_exception("only square matrices are LU decomposition.");
        }

        Self q = this->clone();
        for (usize k = 0; k < q.rows_ - 1; ++k) {
            check_pivot(q(k, k));
            for (usize i = k + 1; i < rows_; ++i) {
                q(i, k) /= q(k, k);
                for (usize j = k + 1; j < cols_; ++j) {
                    q(i, j) -= q(i, k) * q(k, j);
                }
            }
        }

        Self l(rows_, cols_), u(rows_, cols_);
        for (usize i = 0; i < rows_; ++i) {
            for (usize j = 0; j < i; ++j) {
                l(i, j) = q(i, j);
                u(i, j) = 0;
            }
            l(i, i) = 1;
            u(i, i) = q(i, i);
            for (usize j = i + 1; j < cols_; ++j) {
                l(i, j) = 0;
                u(i, j) = q(i, j);
            }
        }
        return Pair{l, u};
    }

    [[nodiscard]] CString __str__() const {
        std::stringstream stream;
        stream << '[';
        for (usize i = 0; i < rows_; ++i) {
            stream << '[';
            for (usize j = 0; j < cols_; ++j) {
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
            throw runtime_exception("only matrices of the same dimension are comparable");
        }
        usize size = this->data_.size();
        for (usize i = 0; i < size; ++i) {
            if (fcmp(this->data_[i], other.data_[i]) > 0) {
                return 1;
            } else if (fcmp(this->data_[i], other.data_[i]) < 0) {
                return -1;
            }
        }
        return 0;
    }

private:
    value_t& at_impl(usize i, usize j) {
        return data_.at(i * cols_ + j);
    }

    const value_t& at_impl(usize i, usize j) const {
        return data_.at(i * cols_ + j);
    }

    /**
     * @brief 校验高斯-约当消元法的主元，不能为0
     */
    static void check_pivot(value_t pivot) {
        if (is_zero(pivot)) {
            throw runtime_exception("pivot entries cannot be 0.");
        }
    }

    void correct() {
        usize size = data_.size();
        for (usize i = 0; i < size; ++i) {
            data_[i] = correct_float(data_[i]);
        }
    }

private:
    usize rows_;              // 行数
    usize cols_;              // 列数
    util::Vec<value_t> data_; // 一维存储，提高空间局部性
};

} // namespace my::math

#endif // MATRIX_HPP