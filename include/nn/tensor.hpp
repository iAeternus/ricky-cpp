/**
 * @brief 张量，N维数组
 * @author Ricky
 * @date 2026/5/29
 * @version 1.0
 */
#ifndef TENSOR_HPP
#define TENSOR_HPP

#include "math_concepts.hpp"
#include "random.hpp"

namespace my::nn {

template <typename T, typename Alloc = mem::Allocator<T>>
class Tensor : public Object<Tensor<T, Alloc>> {
public:
    using Self = Tensor<T, Alloc>;
    using UsizeAlloc = typename Alloc::template rebind<usize>::other;
    using Storage = util::Vec<T, Alloc>;
    using Shape = util::Vec<usize, UsizeAlloc>;

    /**
     * @brief 构造空张量
     */
    Tensor() = default;

    /**
     * @brief 构造连续张量
     * @param shape 张量形状
     */
    explicit Tensor(const Shape& shape) :
            shape_(shape),
            stride_(make_stride(shape_)) {
        this->numel_ = calc_numel(shape_);
        this->data_ = std::make_shared<Storage>(numel_);
    }

    /**
     * @brief 构造连续张量
     * @param shape 张量形状
     */
    explicit Tensor(Shape&& shape) :
            shape_(std::move(shape)),
            stride_(make_stride(shape_)) {
        this->numel_ = calc_numel(shape_);
        this->data_ = std::make_shared<Storage>(numel_);
    }

    /**
     * @brief 使用指定值填充张量
     * @param shape 张量形状
     * @param value 初始值
     */
    Tensor(const Shape& shape, const T& value) :
            shape_(shape),
            stride_(make_stride(shape_)) {
        this->numel_ = calc_numel(shape_);
        this->data_ = std::make_shared<Storage>(numel_, value);
    }

    /**
     * @brief 创建全0张量
     * @param shape 张量形状
     * @return 张量
     */
    static Self zeros(const Shape& shape) {
        return Tensor(shape, static_cast<T>(0));
    }

    /**
     * @brief 创建全1张量
     * @param shape 张量形状
     * @return 张量
     */
    static Self ones(const Shape& shape) {
        return Tensor(shape, static_cast<T>(1));
    }

    /**
     * @brief 创建随机张量
     * @param shape 张量形状
     * @return 张量
     */
    static Self rand(const Shape& shape) {
        auto tensor = Self(shape);
        auto& rng = util::Random::thread_local_rng();
        for (auto& v : *tensor.data_) {
            v = rng.gen<T>();
        }
        return tensor;
    }

    /**
     * @brief 创建正态分布随机张量
     * @param shape 张量形状
     * @param mean 均值
     * @param stddev 标准差
     * @return 张量
     */
    static Self randn(const Shape& shape, T mean = static_cast<T>(0), T stddev = static_cast<T>(1)) {
        static_assert(math::FloatingPointType<T>, "Tensor::randn requires floating point type");

        auto tensor = Self(shape);
        auto& rng = util::Random::thread_local_rng();
        for (auto& v : *tensor.data_) {
            v = rng.normal(mean, stddev);
        }
        return tensor;
    }

    /**
     * @brief 创建等差张量
     * @param start 起始值
     * @param end 终止值
     * @param step 步长
     * @return 张量
     * @exception Exception 若步长为0，则抛出 argument_exception
     */
    static Self arange(T start, T end, T step = static_cast<T>(1)) {
        if (step == static_cast<T>(0)) {
            throw argument_exception("Tensor::arange requires step != 0");
        }

        if ((step > 0 && start >= end) || (step < 0 && start <= end)) {
            return Self(Shape{0});
        }

        const auto size =
            static_cast<usize>(std::ceil(static_cast<f64>(end - start) / static_cast<f64>(step)));

        auto tensor = Self(Shape{size});
        T value = start;
        for (usize i = 0; i < size; ++i) {
            (*tensor.data_)[i] = value;
            value += step;
        }
        return tensor;
    }

    /**
     * @brief 获取维度数
     * @return 维度数
     */
    [[nodiscard]] usize ndim() const noexcept {
        return shape_.len();
    }

    /**
     * @brief 获取元素个数
     * @return 元素个数
     */
    [[nodiscard]] usize numel() const noexcept {
        return numel_;
    }

    /**
     * @brief 获取张量形状
     * @return 张量形状
     */
    [[nodiscard]] const Shape& shape() const noexcept {
        return shape_;
    }

    /**
     * @brief 获取步长
     * @return 步长
     */
    [[nodiscard]] const Shape& stride() const noexcept {
        return stride_;
    }

    /**
     * @brief 获取偏移量
     * @return 偏移量
     */
    [[nodiscard]] usize offset() const noexcept {
        return offset_;
    }

    /**
     * @brief 判断是否为空张量
     * @return 是否为空
     */
    [[nodiscard]] bool is_empty() const noexcept {
        return data_->is_empty();
    }

    /**
     * @brief 判断是否为连续内存
     * @return 是否连续
     */
    [[nodiscard]] bool is_contiguous() const noexcept {
        if (shape_.is_empty()) {
            return true;
        }

        usize expected = 1;
        for (isize i = static_cast<isize>(shape_.len()) - 1; i >= 0; --i) {
            if (stride_[i] != expected) {
                return false;
            }
            expected *= shape_[i];
        }
        return true;
    }

    // /**
    //  * @brief 判断是否为view张量
    //  * @return 是否为view
    //  */
    // [[nodiscard]] bool is_view() const noexcept;

    /**
     * @brief 获取底层数据指针
     * @return 数据指针
     */
    [[nodiscard]] T* data() noexcept {
        return data_->data() + offset_;
    }

    /**
     * @brief 获取底层数据指针
     * @return 数据指针
     */
    [[nodiscard]] const T* data() const noexcept {
        return data_->data() + offset_;
    }

    /**
     * @brief 获取底层数据引用
     */

    Storage& storage() noexcept {
        return *data_;
    }

    /**
     * @brief 获取底层数据引用
     */
    const Storage& storage() const noexcept {
        return *data_;
    }

    /**
     * @brief 多维索引访问
     * @param indices 多维坐标
     * @return 元素引用
     */
    template <math::IntegerType... Args>
    T& operator()(Args&&... indices) {
        return (*data_)[calc_offset(std::forward<Args>(indices)...)];
    }

    /**
     * @brief 多维索引访问
     * @param indices 多维坐标
     * @return 元素引用
     */
    template <math::IntegerType... Args>
    const T& operator()(Args&&... indices) const {
        return (*data_)[calc_offset(std::forward<Args>(indices)...)];
    }

    /**
     * @brief reshape张量
     * @param shape 新形状
     * @return view张量
     * @exception Exception 若张量不连续，则抛出 tensor_exception
     * @exception Exception 若给定shape对应张量元素个数与原有张量不匹配，则抛出 tensor_exception
     *
     * 不复制数据
     */
    [[nodiscard]] Self view(const Shape& shape) const {
        if (!is_contiguous()) {
            throw tensor_exception("Tensor::view requires contiguous tensor");
        }

        if (calc_numel(shape) != numel_) {
            throw tensor_exception("Tensor::view numel mismatch");
        }

        Self result;
        result.data_ = data_;
        result.shape_ = shape;
        result.stride_ = make_stride(shape);
        result.offset_ = offset_;
        result.numel_ = numel_;
        return result;
    }

    /**
     * @brief reshape张量
     * @param shape 新形状
     * @return view张量
     * @exception Exception 若给定shape对应张量元素个数与原有张量不匹配，则抛出 tensor_exception
     *
     * 若内存不连续则复制数据
     */
    [[nodiscard]] Self reshape(const Shape& shape) const {
        if (calc_numel(shape) != numel_) {
            throw tensor_exception("Tensor::reshape numel mismatch");
        }

        if (is_contiguous()) {
            return view(shape);
        }

        return contiguous().view(shape);
    }

    /**
     * @brief 转置两个维度
     * @param dim0 第一个维度
     * @param dim1 第二个维度
     * @return view张量
     * @exception Exception 若给定dim越界，则抛出 tensor_exception
     */
    [[nodiscard]] Self transpose(usize dim0, usize dim1) const {
        if (dim0 >= ndim() || dim1 >= ndim()) {
            throw tensor_exception("Tensor::transpose dimension out of range");
        }

        Self result = *this;

        std::swap(result.shape_[dim0], result.shape_[dim1]);
        std::swap(result.stride_[dim0], result.stride_[dim1]);

        return result;
    }

    /**
     * @brief 重排维度
     * @param dims 新维度顺序
     * @return view张量
     * @exception Exception 若给定dims长度与原张量ndim不匹配，则抛出 tensor_exception
     * @exception Exception 若给定dims中任意维度>=原张量ndim或存在重复维度，则抛出 tensor_exception
     */
    [[nodiscard]] Self permute(const Shape& dims) const {
        if (dims.len() != ndim()) {
            throw tensor_exception("Tensor::permute dimension mismatch");
        }

        Self result;
        result.data_ = data_;
        result.offset_ = offset_;
        result.numel_ = numel_;
        auto size = ndim();
        result.shape_.reserve(size);
        result.stride_.reserve(size);

        util::Vec<bool> used(size, false);
        for (usize d : dims) {
            if (d >= ndim() || used[d]) {
                throw tensor_exception("Tensor::permute invalid dims");
            }
            used[d] = true;
            result.shape_.push(shape_[d]);
            result.stride_.push(stride_[d]);
        }
        return result;
    }

    /**
     * @brief 获取切片
     * @param dim 维度
     * @param start 起始下标
     * @param end 结束下标，不包含
     * @return view张量
     * @exception Exception 若给定dim越界，则抛出 tensor_exception
     * @exception Exception 若给定start或end非法，则抛出 tensor_exception
     */
    [[nodiscard]] Self slice(usize dim, usize start, usize end) const {
        if (dim >= ndim()) {
            throw tensor_exception("Tensor::slice dim out of range");
        }

        if (start > end || end > shape_[dim]) {
            throw tensor_exception("Tensor::slice invalid range");
        }

        Self result = *this;
        result.offset_ += start * stride_[dim];
        result.shape_[dim] = end - start;
        result.numel_ = calc_numel(result.shape_);
        return result;
    }

    /**
     * @brief 展平张量
     * @return view张量
     */
    [[nodiscard]] Self flatten() const {
        return reshape(Shape{numel_});
    }

    /**
     * @brief 插入长度为1的维度
     * @param dim 插入位置
     * @return view张量
     * @exception Exception 若给定dim越界，则抛出 tensor_exception
     */
    [[nodiscard]] Self unsqueeze(usize dim) const {
        if (dim > ndim()) {
            throw tensor_exception("Tensor::unsqueeze invalid dim");
        }

        Self result = *this;
        result.shape_.insert(dim, 1);
        usize stride = dim >= ndim() ? 1 : result.stride_[dim];
        result.stride_.insert(dim, stride);
        return result;
    }

    /**
     * @brief 删除长度为1的维度
     * @param dim 删除位置
     * @return view张量
     * @exception Exception 若给定dim越界，则抛出 tensor_exception
     * @exception Exception 若给定dim对应维度不为1，则抛出 tensor_exception
     */
    [[nodiscard]] Self squeeze(usize dim) const {
        if (dim >= ndim()) {
            throw tensor_exception("Tensor::squeeze invalid dim");
        }

        if (shape_[dim] != 1) {
            throw tensor_exception("Tensor::squeeze requires dim == 1");
        }

        Self result = *this;
        result.shape_.pop(dim);
        result.stride_.pop(dim);
        return result;
    }

    /**
     * @brief 删除所有长度为1的维度
     * @return view张量
     */
    [[nodiscard]] Self squeeze() const {
        Self result = *this;

        for (isize i = static_cast<isize>(result.ndim()) - 1; i >= 0; --i) {
            if (result.shape_[i] == 1) {
                result.shape_.pop(i);
                result.stride_.pop(i);
            }
        }

        return result;
    }

    /**
     * @brief 转换为连续内存
     * @return 连续张量
     */
    [[nodiscard]] Self contiguous() const {
        if (is_contiguous()) {
            return *this;
        }

        Self result(shape_);
        Shape indices(shape_.len(), 0);
        for (usize i = 0; i < numel_; ++i) {
            usize src_offset = offset_;
            for (usize d = 0; d < shape_.len(); ++d) {
                src_offset += indices[d] * stride_[d];
            }
            (*result.data_)[i] = (*data_)[src_offset];

            // 进位
            for (isize d = static_cast<isize>(shape_.len()) - 1; d >= 0; --d) {
                indices[d]++;
                if (indices[d] < shape_[d]) {
                    break;
                }
                indices[d] = 0;
            }
        }
        return result;
    }

    /**
     * @brief 张量加法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self operator+(const Self& rhs) const {
        return elementwise(*this, rhs, [](const T& a, const T& b) {
            return a + b;
        });
    }

    /**
     * @brief 张量减法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self operator-(const Self& rhs) const {
        return elementwise(*this, rhs, [](const T& a, const T& b) {
            return a - b;
        });
    }

    /**
     * @brief 张量乘法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self operator*(const Self& rhs) const {
        return elementwise(*this, rhs, [](const T& a, const T& b) {
            return a * b;
        });
    }

    /**
     * @brief 张量除法
     * @param rhs 右操作数
     * @return 结果张量
     * @exception tensor_exception 除以零时抛出
     */
    [[nodiscard]] Self operator/(const Self& rhs) const {
        return elementwise(*this, rhs, [](const T& a, const T& b) {
            if constexpr (std::is_floating_point_v<T>) {
                if (b == static_cast<T>(0)) {
                    throw tensor_exception("Tensor division by zero");
                }
            } else {
                if (b == static_cast<T>(0)) {
                    throw tensor_exception("Tensor division by zero");
                }
            }
            return a / b;
        });
    }

    /**
     * @brief 广播加法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self broadcast_add(const Self& rhs) const {
        return elementwise_with_boradcase(*this, rhs, [](const T& a, const T& b) {
            return a + b;
        });
    }

    /**
     * @brief 广播减法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self broadcast_sub(const Self& rhs) const {
        return elementwise_with_boradcase(*this, rhs, [](const T& a, const T& b) {
            return a - b;
        });
    }

    /**
     * @brief 广播乘法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self broadcast_mul(const Self& rhs) const {
        return elementwise_with_boradcase(*this, rhs, [](const T& a, const T& b) {
            return a * b;
        });
    }

    /**
     * @brief 广播除法
     * @param rhs 右操作数
     * @return 结果张量
     */
    [[nodiscard]] Self broadcast_div(const Self& rhs) const {
        return elementwise_with_boradcase(*this, rhs, [](const T& a, const T& b) {
            if constexpr (std::is_floating_point_v<T>) {
                if (b == static_cast<T>(0)) {
                    throw tensor_exception("Tensor division by zero");
                }
            } else {
                if (b == static_cast<T>(0)) {
                    throw tensor_exception("Tensor division by zero");
                }
            }
            return a / b;
        });
    }

    /**
     * @brief 矩阵乘法
     * @param rhs 右操作数
     * @return 结果张量
     */
    // [[nodiscard]] Self matmul(const Self& rhs) const {
    //     if (ndim() < 1 || rhs.ndim() < 1) {
    //         throw tensor_exception("matmul requires at least 1D tensors");
    //     }

    //     usize a_nd = ndim();
    //     usize b_nd = rhs.ndim();

    //     usize M = shape_[a_nd - 2];
    //     usize K = shape_[a_nd - 1];
    //     usize K2 = rhs.shape_[b_nd - 2];
    //     usize N = rhs.shape_[b_nd - 1];

    //     if (K != K2) {
    //         throw tensor_exception("matmul shape mismatch on K dimension");
    //     }

    //     // broadcast batch shape
    //     Shape a_batch(shape_.begin(), shape_.end() - 2);
    //     Shape b_batch(rhs.shape_.begin(), rhs.shape_.end() - 2);

    //     Shape out_batch = broadcast_shape(a_batch, b_batch);

    //     Shape out_shape = out_batch;
    //     out_shape.push(M);
    //     out_shape.push(N);

    //     Self result(out_shape);

    //     usize batch_size = calc_numel(out_batch);

    //     Shape batch_idx(out_batch.len(), 0);

    //     auto advance = [&](Shape& idx, const Shape& shape) {
    //         for (isize i = (isize)shape.len() - 1; i >= 0; --i) {
    //             idx[i]++;
    //             if (idx[i] < shape[i]) return;
    //             idx[i] = 0;
    //         }
    //     };

    //     for (usize b = 0; b < batch_size; ++b) {
    //         usize a_base = offset_;
    //         usize b_base = rhs.offset_;

    //         // map batch index -> offset
    //         for (usize i = 0; i < out_batch.len(); ++i) {
    //             usize ai = (i < a_batch.len()) ? batch_idx[i] : 0;
    //             usize bi = (i < b_batch.len()) ? batch_idx[i] : 0;

    //             if (i < a_batch.len())
    //                 a_base += ai * stride_[i];

    //             if (i < b_batch.len())
    //                 b_base += bi * rhs.stride_[i];
    //         }

    //         // matrix multiply
    //         for (usize i = 0; i < M; ++i) {
    //             for (usize j = 0; j < N; ++j) {
    //                 T sum = T{0};

    //                 for (usize k = 0; k < K; ++k) {
    //                     sum += (*data_)[a_base + i * stride_[a_nd - 2] + k * stride_[a_nd - 1]]
    //                            * (*rhs.data_)[b_base + k * rhs.stride_[b_nd - 2] + j * rhs.stride_[b_nd - 1]];
    //                 }

    //                 result(batch_idx, i, j) = sum;
    //             }
    //         }

    //         advance(batch_idx, out_batch);
    //     }

    //     return result;
    // }

    /**
     * @brief 求和
     * @return 标量
     */
    [[nodiscard]] T sum() const {
        auto t = contiguous();
        T acc = 0;

        for (usize i = 0; i < numel_; ++i) {
            acc += (*t.data_)[i];
        }
        return acc;
    }

    /**
     * @brief 求均值
     * @return 标量
     */
    [[nodiscard]] T mean() const {
        if (numel_ == 0) {
            throw tensor_exception("mean of empty tensor");
        }

        return sum() / static_cast<T>(numel_);
    }

    /**
     * @brief 求最大值
     * @return 标量
     */
    [[nodiscard]] T max() const {
        auto t = contiguous();

        if (numel_ == 0) {
            throw tensor_exception("max of empty tensor");
        }

        T m = (*t.data_)[0];
        for (usize i = 1; i < numel_; ++i) {
            m = std::max(m, (*t.data_)[i]);
        }
        return m;
    }

    /**
     * @brief 求最小值
     * @return 标量
     */
    [[nodiscard]] T min() const {
        auto t = contiguous();

        if (numel_ == 0) {
            throw tensor_exception("min of empty tensor");
        }

        T m = (*t.data_)[0];
        for (usize i = 1; i < numel_; ++i) {
            m = std::min(m, (*t.data_)[i]);
        }
        return m;
    }

    /**
     * @brief 比较两个张量
     * @param other 另一个张量
     * @return 比较结果
     *
     * 比较顺序：
     *
     * 1. shape
     * 2. stride
     * 3. offset
     * 4. 元素值（逻辑顺序）
     */
    [[nodiscard]] auto cmp(const Self& other) const -> cmp_t {
        if (auto c = shape_.cmp(other.shape_); c != 0) {
            return c;
        }

        if (auto c = stride_.cmp(other.stride_); c != 0) {
            return c;
        }

        if (offset_ < other.offset_) {
            return -1;
        }

        if (offset_ > other.offset_) {
            return 1;
        }

        // 比较逻辑元素值
        Shape lhs_indices(shape_.len(), 0);
        Shape rhs_indices(other.shape_.len(), 0);

        for (usize i = 0; i < numel_; ++i) {
            usize lhs_offset = offset_;
            usize rhs_offset = other.offset_;

            for (usize d = 0; d < shape_.len(); ++d) {
                lhs_offset += lhs_indices[d] * stride_[d];
                rhs_offset += rhs_indices[d] * other.stride_[d];
            }

            const auto& lhs = (*data_)[lhs_offset];
            const auto& rhs = (*other.data_)[rhs_offset];

            if (lhs < rhs) {
                return -1;
            }

            if (lhs > rhs) {
                return 1;
            }

            // 多维进位
            for (isize d = static_cast<isize>(shape_.len()) - 1; d >= 0; --d) {
                lhs_indices[d]++;
                rhs_indices[d]++;

                if (lhs_indices[d] < shape_[d]) {
                    break;
                }

                lhs_indices[d] = 0;
                rhs_indices[d] = 0;
            }
        }

        return 0;
    }

    /**
     * @brief 转换为字符串
     * @return 字符串表示
     *
     * 示例：
     *
     * Tensor(shape=[2,3], stride=[3,1], data=[[1,2,3],[4,5,6]])
     */
    [[nodiscard]] auto to_string() const -> CString {
        std::stringstream stream;

        stream << "Tensor(shape=";
        stream << shape_.to_string();

        stream << ", stride=";
        stream << stride_.to_string();

        stream << ", offset=";
        stream << std::to_string(offset_);

        stream << ", data=";

        if (shape_.is_empty()) {
            // scalar
            stream << std::format("{}", (*data_)[offset_]);
        } else {
            Shape indices(shape_.len(), 0);

            std::function<void(usize)> dfs = [&](usize dim) {
                stream << "[";
                if (dim + 1 == shape_.len()) {
                    for (usize i = 0; i < shape_[dim]; ++i) {
                        indices[dim] = i;
                        usize off = offset_;
                        for (usize d = 0; d < shape_.len(); ++d) {
                            off += indices[d] * stride_[d];
                        }
                        stream << std::format("{}", (*data_)[off]);
                        if (i + 1 != shape_[dim]) {
                            stream << ",";
                        }
                    }
                } else {
                    for (usize i = 0; i < shape_[dim]; ++i) {
                        indices[dim] = i;
                        dfs(dim + 1);
                        if (i + 1 != shape_[dim]) {
                            stream << ",";
                        }
                    }
                }
                stream << "]";
            };

            dfs(0);
        }

        stream << ")";
        return CString{stream.str()};
    }

private:
    /**
     * @brief 计算连续内存步长
     * @param shape 张量形状
     * @return 步长
     *
     * 例如：
     *
     * shape:
     * [2,3,4]
     *
     * stride:
     * [12,4,1]
     */
    [[nodiscard]] static Shape make_stride(const Shape& shape) {
        Shape stride(shape.len());
        usize acc = 1;
        for (isize i = static_cast<isize>(shape.len()) - 1; i >= 0; --i) {
            stride[i] = acc;
            acc *= shape[i];
        }
        return stride;
    }

    /**
     * @brief 计算元素个数
     * @param shape 张量形状
     * @return 元素个数
     */
    [[nodiscard]] static usize calc_numel(const Shape& shape) {
        if (shape.is_empty()) {
            return 1; // 空shape视为标量
        }

        usize numel = 1;
        for (usize dim : shape) {
            numel *= dim;
        }
        return numel;
    }

    /**
     * @brief 计算真实内存下标
     * @param indices 多维坐标
     * @return 一维下标
     * @exception Exception 若张量维度不匹配，则抛出 tensor_exception
     * @exception Exception 若idx[i] >= shape_[i]，则抛出 tensor_exception
     *
     * 公式：
     *
     * offset + sum(indices[i] * stride[i])
     */
    template <math::IntegerType... Args>
    [[nodiscard]] usize calc_offset(Args&&... indices) const {
        static_assert(sizeof...(Args) > 0);
        if (sizeof...(Args) != shape_.len()) {
            throw tensor_exception("Tensor indices dimension mismatch");
        }

        usize idx[] = {static_cast<usize>(indices)...};
        usize off = offset_;
        usize shape_len = shape_.len();
        for (usize i = 0; i < shape_len; ++i) {
            if (idx[i] >= shape_[i]) {
                throw tensor_exception("Tensor index out of range");
            }
            off += idx[i] * stride_[i];
        }
        return off;
    }

    struct BroadcastInfo {
        Shape shape;    // broadcast后的shape
        Shape stride_a; // A在broadcast空间的stride
        Shape stride_b; // B在broadcast空间的stride
    };

    /**
     * @brief 计算两个shape的广播结果
     * @details 从右对齐维度，缺失补1；每维必须相等或其中一个为1，否则抛出 tensor_exception
     * @param a 左shape
     * @param b 右shape
     * @return 广播后的shape
     * @exception tensor_exception 不可广播时抛出
     */
    static Shape broadcast_shape(const Shape& a, const Shape& b) {
        usize na = a.len();
        usize nb = b.len();
        usize n = std::max(na, nb);

        Shape result(n);

        for (isize i = 1; i <= (isize)n; ++i) {
            usize ai = (i <= na) ? a[na - i] : 1;
            usize bi = (i <= nb) ? b[nb - i] : 1;

            if (ai != bi && ai != 1 && bi != 1) {
                throw tensor_exception("broadcast shape mismatch");
            }

            result[n - i] = std::max(ai, bi);
        }

        return result;
    }

    /**
     * @brief 计算广播后的stride映射
     *
     * @details 对齐target_shape：
     * - size==1的维度stride置0
     * - 缺失维度stride置0
     * - 其余维度保持原stride
     *
     * @param shape 原shape
     * @param target_shape 广播目标shape
     * @param stride 原stride
     * @return 广播后的stride
     */
    static Shape broadcast_stride(const Shape& shape, const Shape& target_shape, const Shape& stride) {
        usize n = target_shape.len();
        Shape new_stride(n);

        usize offset = n - shape.len();

        for (usize i = 0; i < n; ++i) {
            if (i < offset) {
                new_stride[i] = 0;
            } else {
                if (shape[i - offset] == 1) {
                    new_stride[i] = 0;
                } else {
                    new_stride[i] = stride[i - offset];
                }
            }
        }

        return new_stride;
    }

    /**
     * @brief 逐元素二元广播操作
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @param op 操作逻辑
     * @return 结果张量
     */
    template <typename F>
    static Self elementwise_with_boradcase(const Self& lhs, const Self& rhs, F&& op) {
        auto out_shape = broadcast_shape(lhs.shape_, rhs.shape_);
        Self result(out_shape);
        auto lhs_stride = broadcast_stride(lhs.shape_, out_shape, lhs.stride_);
        auto rhs_stride = broadcast_stride(rhs.shape_, out_shape, rhs.stride_);
        auto ndim = out_shape.len();
        auto total = calc_numel(out_shape);
        Shape idx(ndim, 0);

        for (usize i = 0; i < total; ++i) {
            usize a_off = lhs.offset_;
            usize b_off = rhs.offset_;

            for (usize d = 0; d < ndim; ++d) {
                a_off += idx[d] * lhs_stride[d];
                b_off += idx[d] * rhs_stride[d];
            }

            (*result.data_)[i] = op((*lhs.data_)[a_off], (*rhs.data_)[b_off]);

            for (isize d = (isize)ndim - 1; d >= 0; --d) {
                if (++idx[d] < out_shape[d]) break;
                idx[d] = 0;
            }
        }

        return result;
    }

    /**
     * @brief 逐元素二元操作
     * @param lhs 左操作数
     * @param rhs 右操作数
     * @param op 操作逻辑
     * @return 结果张量
     * @exception Exception 若左右操作数shape不匹配，则抛出 tensor_exception
     */
    template <typename F>
    static Self elementwise(const Self& lhs, const Self& rhs, F&& op) {
        if (lhs.shape_ != rhs.shape_) {
            throw tensor_exception("Tensor shape mismatch in elementwise op");
        }

        Self result(lhs.shape_);
        Shape idx(lhs.ndim(), 0);
        auto lhs_ndim = lhs.ndim();
        for (usize i = 0; i < lhs.numel_; ++i) {
            usize a_off = lhs.offset_;
            usize b_off = rhs.offset_;

            for (usize d = 0; d < lhs_ndim; ++d) {
                a_off += idx[d] * lhs.stride_[d];
                b_off += idx[d] * rhs.stride_[d];
            }

            (*result.data_)[i] = op((*lhs.data_)[a_off], (*rhs.data_)[b_off]);

            // advance multi-index
            for (isize d = (isize)lhs_ndim - 1; d >= 0; --d) {
                idx[d]++;
                if (idx[d] < lhs.shape_[d]) break;
                idx[d] = 0;
            }
        }

        return result;
    }

private:
    std::shared_ptr<Storage> data_;
    Shape shape_;
    Shape stride_;
    usize offset_ = 0;
    usize numel_ = 1;
};

} // namespace my::nn

#endif // TENSOR_HPP
